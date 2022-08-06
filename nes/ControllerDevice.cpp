#include "ControllerDevice.h"
#include "../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

ControllerState::ControllerState() {
	controller1Latch = CONTROLLER_INITIAL_VALUE;
	controller2Latch = CONTROLLER_INITIAL_VALUE;
	latchFillToggle = false;
}

ControllerDevice::ControllerDevice(Environment* environment) : BusDevice(std::list({AddressRange(CONTROLLER_BEGIN_ADDRESS, CONTROLLER_END_ADDRESS)})), m_environment(environment)
{
	m_controller1Latch = CONTROLLER_INITIAL_VALUE;
	m_controller2Latch = CONTROLLER_INITIAL_VALUE;
	m_latchFillToggle = false;
}

void ControllerDevice::SoftReset()
{
	m_controller1Latch = CONTROLLER_INITIAL_VALUE;
	m_controller2Latch = CONTROLLER_INITIAL_VALUE;
	m_latchFillToggle = false;
}

void ControllerDevice::HardReset()
{
	m_controller1Latch = CONTROLLER_INITIAL_VALUE;
	m_controller2Latch = CONTROLLER_INITIAL_VALUE;
	m_latchFillToggle = false;
}

uint8_t ControllerDevice::Read(uint16_t address)
{
	if (address == CONTROLLER_1_ADDRESS) {
		return this->GetLatchBit(NES_CONTROLLER_1);
	}
	else {
		return this->GetLatchBit(NES_CONTROLLER_2);
	}
}

void ControllerDevice::Write(uint16_t address, uint8_t data)
{
	if (address == CONTROLLER_1_ADDRESS) {
		m_latchFillToggle = data & 0x1;
		this->FillLatches();
	}
	// Writes to 0x4017 are meant to go to the APU
}

uint8_t ControllerDevice::Probe(uint16_t address)
{
	if (address == CONTROLLER_1_ADDRESS) {
		return m_controller1Latch;
	}
	else {
		return m_controller2Latch;
	}
}

ControllerState ControllerDevice::GetState() const
{
	ControllerState state;
	state.controller1Latch = m_controller1Latch;
	state.controller2Latch = m_controller2Latch;
	state.latchFillToggle = m_latchFillToggle;
	return state;
}

void ControllerDevice::LoadState(ControllerState& state)
{
	m_controller1Latch = state.controller1Latch;
	m_controller2Latch = state.controller2Latch;
	state.latchFillToggle = m_latchFillToggle;
	// Important - not filling up the latch when loading state, regardless of toggle value
}

void ControllerDevice::FillLatches()
{
	if (m_environment != nullptr) {
		m_environment->GetControllerInputs(m_controller1Latch, m_controller2Latch);
	}
}

uint8_t ControllerDevice::GetLatchBit(NES_CONTROLLER_ID controller)
{
	if (m_latchFillToggle) {
		// If the latch fill toggle is still on, we need to return what is currently being pressed
		// This means that as long as it's not turned off, we always return the status of the A key
		this->FillLatches();
	}
	uint8_t returnValue = 1;
	switch (controller) {
	case NES_CONTROLLER_1:
		returnValue = TestBit8(m_controller1Latch, 0);
		m_controller1Latch = ShiftRight8(m_controller1Latch, 1);
		SetBit8(m_controller1Latch, 7); // Mimics behaviour of standard NES controller
		break;
	case NES_CONTROLLER_2:
		returnValue = TestBit8(m_controller2Latch, 0);
		m_controller2Latch = ShiftRight8(m_controller2Latch, 1);
		SetBit8(m_controller2Latch, 7); // Mimics behaviour of standard NES controller
		break;
	}
	return returnValue;
}
