#include <assert.h>

#include "Environment.h"
#include "utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Environment::Environment() {
	m_display = nullptr;
	m_controller1Latch = 0;
	m_controller2Latch = 0;
}

void Environment::SetDisplayPanel(wxDisplayPanel* display) {
	m_display = display;
}

void Environment::UpdateDisplay(const NESPicture& picture) {
	assert(m_display != nullptr);
	m_display->SetImage(picture);
}

void Environment::HandleNESKeyDown(NES_CONTROLLER_ID controller, NES_CONTROLLER_KEY key) {
	wxCriticalSectionLocker enter(m_controllerCritSection);
	if (controller == NES_CONTROLLER_1) {
		SetBit8(m_controller1Latch, key);
	}
	else {
		SetBit8(m_controller2Latch, key);
	}
}

void Environment::HandleNESKeyUp(NES_CONTROLLER_ID controller, NES_CONTROLLER_KEY key) {
	wxCriticalSectionLocker enter(m_controllerCritSection);
	if (controller == NES_CONTROLLER_1) {
		ClearBit8(m_controller1Latch, key);
	}
	else {
		ClearBit8(m_controller2Latch, key);
	}
}

void Environment::GetControllerInputs(uint8_t& controller1Latch, uint8_t& controller2Latch) {
	{
		wxCriticalSectionLocker enter(m_controllerCritSection);
		controller1Latch = m_controller1Latch;
		controller2Latch = m_controller2Latch;
	}
	this->ResolveDPadConflicts(controller1Latch);
	this->ResolveDPadConflicts(controller2Latch);
	
}

void Environment::ResolveDPadConflicts(uint8_t& controllerLatch) {
	// Do not allow Up & Down to be held together. Same for Right & Left
	if (TestBit8(controllerLatch, NES_KEY_UP) && TestBit8(controllerLatch, NES_KEY_DOWN)) {
		ClearBit8(controllerLatch, NES_KEY_DOWN);
	}
	if (TestBit8(controllerLatch, NES_KEY_RIGHT) && TestBit8(controllerLatch, NES_KEY_LEFT)) {
		ClearBit8(controllerLatch, NES_KEY_LEFT);
	}
}