#include <assert.h>

#include "Environment.h"
#include "utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Environment::Environment() {
	m_display = nullptr;
	/*
	m_controller1Latch = 0;
	m_controller2Latch = 0;
	*/

	// Controller 1
	m_nesKeyTranslator['X'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_A);
	m_nesKeyTranslator['Z'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_B);
	m_nesKeyTranslator['A'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_SELECT);
	m_nesKeyTranslator['S'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_START);
	m_nesKeyTranslator[WXK_UP] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_UP);
	m_nesKeyTranslator[WXK_DOWN] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_DOWN);
	m_nesKeyTranslator[WXK_LEFT] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_LEFT);
	m_nesKeyTranslator[WXK_RIGHT] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_1, NES_KEY_RIGHT);

	// Controller 2

	m_nesKeyTranslator['V'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_A);
	m_nesKeyTranslator['C'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_B);
	m_nesKeyTranslator['D'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_SELECT);
	m_nesKeyTranslator['F'] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_START);
	m_nesKeyTranslator[WXK_NUMPAD8] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_UP);
	m_nesKeyTranslator[WXK_NUMPAD2] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_DOWN);
	m_nesKeyTranslator[WXK_NUMPAD4] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_LEFT);
	m_nesKeyTranslator[WXK_NUMPAD6] = std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>(NES_CONTROLLER_2, NES_KEY_RIGHT);
}

void Environment::SetDisplayPanel(wxDisplayPanel* display) {
	m_display = display;
}

void Environment::UpdateDisplay(const NESPicture& picture) {
	assert(m_display != nullptr);
	m_display->SetImage(picture);
}

/*
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
*/

void Environment::GetControllerInputs(uint8_t& controller1Latch, uint8_t& controller2Latch) {
	/*
	{
		wxCriticalSectionLocker enter(m_controllerCritSection);
		controller1Latch = m_controller1Latch;
		controller2Latch = m_controller2Latch;
	}
	*/
	for (std::pair<const int, std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>>& keyPressInfo : m_nesKeyTranslator) {
		if (wxGetKeyState((wxKeyCode)keyPressInfo.first)) {
			keyPressInfo.second.first == NES_CONTROLLER_1 ? SetBit8(controller1Latch, keyPressInfo.second.second) : SetBit8(controller2Latch, keyPressInfo.second.second);
		}
		else {
			keyPressInfo.second.first == NES_CONTROLLER_1 ? ClearBit8(controller1Latch, keyPressInfo.second.second) : ClearBit8(controller2Latch, keyPressInfo.second.second);
		}
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