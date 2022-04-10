/*
 * RAMDevice.cpp
 *
 *  Created on: 30 Jan 2022
 *      Author: yaniv
 */

#include "RAMDevice.h"
#include "BitwiseUtils.h"


RAMState::RAMState() {
	content.fill(0);
}

RAMDevice::RAMDevice() : BusDevice(std::list<AddressRange>({
		AddressRange(RAM_ADDRESS_START,RAM_ADDRESS_END) })) {
	m_RAMContent.fill(0);
}

// Values of RAM unchanged on soft reset
void RAMDevice::SoftReset() {}

void RAMDevice::HardReset() {
	// TODO: Initialise RAM randomly on reset for Final Fantasy
	m_RAMContent.fill(0);
	m_RAMContent[0x401] = 0x2;
}

// TODO: use util to do masking
uint8_t RAMDevice::Read(uint16_t address) {
	return m_RAMContent[BitwiseUtils::ClearUpperBits16(address - RAM_ADDRESS_START, 11)];
}

// TODO: use util to do masking
void RAMDevice::Write(uint16_t address, uint8_t data) {
	m_RAMContent[BitwiseUtils::ClearUpperBits16(address - RAM_ADDRESS_START, 11)] = data;
}

RAMState RAMDevice::GetState() const {
	RAMState state;
	state.content = m_RAMContent;
	return state;
}

void RAMDevice::LoadState(RAMState& state) {
	m_RAMContent = state.content;
}

