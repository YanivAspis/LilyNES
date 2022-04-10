/*
 * CPU2A03.cpp
 *
 *  Created on: 30 Jan 2022
 *      Author: yaniv
 */

#include "CPU2A03.h"
#include "BitwiseUtils.h"
#include <assert.h>

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
	B = 1; // Mimics behaviour of actual NES
	Unused = 1; // Mimics behaviour of actual NES
	V = 0;
	N = 0;
}

CPUState::CPUState() {
	regA = 0;
	regX = 0;
	regY = 0;
	regS = 0;
	regPC = 0;
	regP.value = 0;
}



CPU2A03::CPU2A03() {
	m_cpuBus = nullptr;

	// Just sets Regs to 0 in c'tor. Call HardReset to change
	// to correct Power Up values.
	m_regA = 0;
	m_regX = 0;
	m_regY = 0;
	m_regS = 0;
	m_regPC = 0;
	m_regP.value = 0;
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
	return state;

}

void CPU2A03::LoadState(CPUState& state) {
	m_regA = state.regA;
	m_regX = state.regX;
	m_regY = state.regY;
	m_regS = state.regS;
	m_regPC = state.regPC;
	m_regP.value = state.regP.value;
}

void CPU2A03::Clock()
{
	BitwiseUtils::Inc8Bit(m_regA);
	BitwiseUtils::Dec8Bit(m_regX);
	m_regY = 0x50;
	BitwiseUtils::Inc8Bit(m_regP.value);
}

uint16_t CPU2A03::FetchInitialPC() {
	uint8_t lowByte = m_cpuBus->Read(ADDR_RESET_VECTOR_LOW);
	uint8_t highByte = m_cpuBus->Read(ADDR_RESET_VECTOR_HIGH);
	return CombineBytes(lowByte, highByte);
}