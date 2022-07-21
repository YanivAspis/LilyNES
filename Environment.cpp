#include <assert.h>

#include "Environment.h"

void Environment::SetDisplayPanel(wxDisplayPanel* display) {
	m_display = display;
}

void Environment::UpdateDisplay(const NESPicture& picture) {
	assert(m_display != nullptr);
	m_display->SetImage(picture);
}