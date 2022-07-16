#include <assert.h>

#include "NES.h"
#include "mappers/Mapper000.h"


NES::NES(): m_cpu(false) {
	m_cpuBus.ConnectDevice(&m_RAM);
	m_cpu.ConnectToBus(&m_cpuBus);
	m_cartridge = nullptr;
}

NES::~NES() {
	m_cpuBus.DisconnectAllDevices();
	if (m_cartridge != nullptr) {
		delete m_cartridge;
		m_cartridge = nullptr;
	}
}

void NES::LoadROM(const INESFile& romFile) {
	if (m_cartridge != nullptr) {
		this->DisconnectCartridge();
	}
	switch (romFile.GetHeader().GetMapperId()) {
	case 0x00:
		m_cartridge = new Mapper000(romFile);
		break;
	default:
		throw UnsupportedMapperException(romFile.GetHeader().GetMapperId());
	}
	this->ConnectCartridge();
}



void NES::SoftReset()
{
	m_cpuBus.SoftReset();
	m_cpu.SoftReset();
}

void NES::HardReset()
{
	m_cpuBus.HardReset();
	m_cpu.HardReset();
}

NESState NES::GetState() const
{
	NESState state;
	state.cpuState = m_cpu.GetState();
	state.ramState = m_RAM.GetState();
	if (m_cartridge != nullptr) {
		state.cartridgeState = m_cartridge->GetState();
	}
	return state;
}

void NES::LoadState(NESState& state)
{
	m_cpu.LoadState(state.cpuState);
	m_RAM.LoadState(state.ramState);
	if (m_cartridge != nullptr) {
		m_cartridge->LoadState(state.cartridgeState);
	}
}

void NES::Clock()
{
	m_cpu.Clock();
}



void NES::StartEmulation() {

}

void NES::PauseEmulation() {

}

void NES::ResumeEmulation() {

}

void NES::StopEmulation() {

}


void NES::RunUntilNextCycle() {
	this->Clock();
}

void NES::RunUntilNextInstruction() {
	while (m_cpu.GetState().cyclesRemaining > 1) {
		this->Clock();
	}
	while (m_cpu.GetState().cyclesRemaining == 1) {
		this->Clock();
	}
}

void NES::RunUntilNextFrame() {

}

void NES::UpdateDisplay() {

}

void NES::RequestControllerInput() {

}



uint8_t NES::ProbeCPUBus(uint16_t address) {
	return m_cpuBus.Probe(address);
}

uint8_t NES::ProbePPUBus(uint16_t address) {
	return 0;
}

CPUState NES::ProbeCPUState() {
	return this->GetState().cpuState;
}

std::vector<uint8_t> NES::ProbeCurrentCPUInstruction() {
	return m_cpu.GetCurrentInstructionBytes();
}

RAMState NES::ProbeRAMState() {
	return this->GetState().ramState;
}

CartridgeState NES::ProbeCartridgeState() {
	return this->GetState().cartridgeState;
}

std::vector<uint8_t> NES::ProbeCurrentPRGRom() {
	return {};
}

std::vector<uint8_t> NES::ProbeCurrentCHRRom() {
	return {};
}


void NES::ConnectCartridge() {
	assert(m_cartridge != nullptr);
	m_cpuBus.ConnectDevice(m_cartridge);
}

void NES::DisconnectCartridge() {
	if (m_cartridge != nullptr) {
		m_cpuBus.DisconnectDevice(m_cartridge);
		delete m_cartridge;
		m_cartridge = nullptr;
	}
}