/*
 * CPU2A03.cpp
 *
 *  Created on: 30 Jan 2022
 *      Author: yaniv
 */

#include <assert.h>

#include "CPU2A03.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;


void CPUStatusFlag::SoftReset() {
	// Only change on reset is disabling interrupts
	I = 1;
}

void CPUStatusFlag::HardReset() {
	C = 0;
	Z = 0;
	I = 1; // Interrupts are disabled on power up
	D = 0;
	B = 0; // Mimics behaviour of actual NES
	Unused = 1; // Mimics behaviour of actual NES
	V = 0;
	N = 0;
}

CurrentInstruction::CurrentInstruction() {
	mnemonic = INSTR_BRK;
	addressMode = MODE_IMPLIED;
	instructionAddress = 0;
	targetAddress = 0;
	accumulatorMode = false;
	length = 0;
	cycles = 0;
	reset = true;
	interrupt = false;
}

CPUState::CPUState() {
	regA = 0;
	regX = 0;
	regY = 0;
	regS = 0;
	regPC = 0;
	regP.value = 0;

	cyclesRemaining = 0;
	irqPending = 0;
	nmiRaised = false;

	instructionFirstCycle = false;
	cycleCount = 0;
}



CPU2A03::CPU2A03(bool decimalAllowed) {
	m_cpuBus = nullptr;
	m_decimalAllowed = decimalAllowed;

	// Just sets Regs to 0 in c'tor. Call HardReset to change
	// to correct Power Up values.
	m_regA = 0;
	m_regX = 0;
	m_regY = 0;
	m_regS = 0;
	m_regPC = 0;
	m_regP.value = 0;

	m_cyclesRemaining = 0;
	m_irqPending = 0;
	m_nmiRaised = false;

	m_currInstruction = CurrentInstruction();
	m_instructionFirstCycle = false;
	m_cycleCount = 0;

	this->PopulateFunctionMaps();
}

CPU2A03::~CPU2A03() {
	// ensure no dangling pointer
	m_cpuBus = nullptr;
}

void CPU2A03::ConnectToBus(Bus* cpuBus) {
	assert(m_cpuBus == nullptr);
	m_cpuBus = cpuBus;
}

void CPU2A03::SoftReset() {

	// Stack pointer decrements by 3 on soft reset
	// even though nothing is pushed
	Dec8Bit(m_regS);
	Dec8Bit(m_regS);
	Dec8Bit(m_regS);

	m_regP.flags.SoftReset();

	m_cyclesRemaining = NUM_RESET_CYCLES;
	m_irqPending = 0;
	m_nmiRaised = false;

	m_instructionFirstCycle = false;
	m_currInstruction = CurrentInstruction();
	m_cycleCount = 0;

	m_regPC = this->FetchInitialPC();
}

void CPU2A03::HardReset() {
	m_regA = 0;
	m_regX = 0;
	m_regY = 0;

	// On power up, stack pointer is set to 0 then
	// decremented by 3 even though nothing is pushed.
	m_regS = SP_INITIAL_VALUE;
	m_regP.flags.HardReset();

	m_cyclesRemaining = NUM_RESET_CYCLES;
	m_irqPending = 0;
	m_nmiRaised = false;

	m_instructionFirstCycle = false;
	m_currInstruction = CurrentInstruction();
	m_cycleCount = 0;

	m_regPC = this->FetchInitialPC();
}

CPUState CPU2A03::GetState() const {
	CPUState state;
	state.regA = m_regA;
	state.regX = m_regX;
	state.regY = m_regY;
	state.regS = m_regS;
	state.regPC = m_regPC;
	state.regP.value = m_regP.value;

	state.cyclesRemaining = m_cyclesRemaining;
	state.irqPending = m_irqPending;
	state.nmiRaised = m_nmiRaised;

	state.instructionFirstCycle = m_instructionFirstCycle;
	state.currInstruction = m_currInstruction;
	state.cycleCount = m_cycleCount;

	return state;
}

void CPU2A03::LoadState(CPUState& state) {
	m_regA = state.regA;
	m_regX = state.regX;
	m_regY = state.regY;
	m_regS = state.regS;
	m_regPC = state.regPC;
	m_regP.value = state.regP.value;

	m_cyclesRemaining = state.cyclesRemaining;
	m_irqPending = state.irqPending;
	m_nmiRaised = state.nmiRaised;

	m_instructionFirstCycle = state.instructionFirstCycle;
	m_currInstruction = state.currInstruction;
	m_cycleCount = state.cycleCount;
}

void CPU2A03::Clock()
{
	// Still performing previous instruction
	m_cyclesRemaining--;
	m_cycleCount++;
	if (m_cyclesRemaining > 0) {
		m_instructionFirstCycle = false;
		return;
	}

	// Reached cycle 0 - actually execute current instruction
	if (!m_currInstruction.reset) {
		if (m_currInstruction.interrupt) {
			// Current instruction was actually IRQ or NMI
			this->HandleInterrupt(false);
		}
		else {
			m_regPC = Add16Bit(m_regPC, m_currInstruction.length);
			m_executeFunctions[m_currInstruction.mnemonic](*this);
		}
	}

	// Set up next instruction
	m_instructionFirstCycle = true;
	//m_currInstruction = CurrentInstruction();
	m_currInstruction.reset = false;

	// Check if an interrupt needs to be handled
	if (m_nmiRaised || (!m_regP.flags.I && m_irqPending > 0)) {
		m_currInstruction.interrupt = true;
		m_cyclesRemaining = NUM_INTERRUPT_CYCLES;
		return;
	}
	
	// No interrupt, perform next instruction
	uint8_t opCode = m_cpuBus->Read(m_regPC);
	CPUInstruction& instruction = s_opCodeTable[opCode];

	m_currInstruction.interrupt = false;
	m_currInstruction.mnemonic = instruction.mnemonic;
	m_currInstruction.addressMode = instruction.addressMode;
	m_currInstruction.instructionAddress = m_regPC;
	m_currInstruction.cycles = instruction.baseCycleCount;
	m_currInstruction.length = instruction.GetInstructionLength();

	// read target address for the instruction to act upon
	m_addressModeFunctions[instruction.addressMode](*this);

	// Fix addressing mode additional cycles for those instructions that don't actually have any
	for (const InstructionMnemonic& mnemonic : CPU2A03::s_mnemonicsWithoutAdditionalCycles) {
		if (mnemonic == instruction.mnemonic) {
			m_currInstruction.cycles = instruction.baseCycleCount;
			break;
		}
	}

	// Check if additional cycles are added due actual execution
	std::map<InstructionMnemonic, std::function<void(CPU2A03&)>>::iterator funcIt = m_additionalExecuteCyclesFunctions.find(instruction.mnemonic);
	if (funcIt != m_additionalExecuteCyclesFunctions.end()) {
		funcIt->second(*this);
	}

	// set amount of cycles to wait until next instruction
	m_cyclesRemaining = m_currInstruction.cycles;
}

void CPU2A03::RaiseIRQ() {
	m_irqPending += 1;
}

void CPU2A03::RaiseNMI() {
	m_nmiRaised = true;
}

std::vector<uint8_t> CPU2A03::GetCurrentInstructionBytes() {
	uint8_t currOpCode = m_cpuBus->Probe(m_currInstruction.instructionAddress);
	std::vector<uint8_t> instructionBytes = { currOpCode };
	for (int i = 1; i < s_opCodeTable[currOpCode].GetInstructionLength(); i++) {
		instructionBytes.push_back(m_cpuBus->Probe(Add16Bit(m_currInstruction.instructionAddress, i)));
	}
	return instructionBytes;
}

uint16_t CPU2A03::FetchInitialPC() {
	uint8_t lowByte = m_cpuBus->Read(ADDR_RESET_VECTOR_LOW);
	uint8_t highByte = m_cpuBus->Read(ADDR_RESET_VECTOR_HIGH);
	return CombineBytes(lowByte, highByte);
}