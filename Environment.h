#pragma once

#include "GUI/wxDisplayPanel.h"

class Environment {
public:
	void SetDisplayPanel(wxDisplayPanel* display);
	void UpdateDisplay(const NESPicture& picture);

private:
	wxDisplayPanel* m_display;
};
