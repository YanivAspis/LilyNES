#pragma once

#include <map>

#include "GUI/wxDisplayPanel.h"
#include "nes/ControllerDevice.h"

enum NES_CONTROLLER_ID;
enum NES_CONTROLLER_KEY;

class Environment {
public:
	Environment();

	void SetDisplayPanel(wxDisplayPanel* display);
	void UpdateDisplay(const NESPicture& picture);
	void SetDisplayMessage(std::string message, double duration);
	void ClearDisplayMessage();

	void GetControllerInputs(uint8_t& controller1Latch, uint8_t& controller2Latch);

private:
	void ResolveDPadConflicts(uint8_t& controllerLatch);

	wxDisplayPanel* m_display;
	std::map < int, std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>> m_nesKeyTranslator;
	uint8_t m_controller1Latch;
	uint8_t m_controller2Latch;
	wxCriticalSection m_controllerCritSection;
};
