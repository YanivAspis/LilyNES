#include "LilyNESApp.h"

wxIMPLEMENT_APP(LilyNESApp);


LilyNESApp::LilyNESApp() {
	m_mainFrame = new wxMainFrame();
	Bind(wxEVT_KEY_DOWN, &LilyNESApp::OnKeyPress, this);
}

bool LilyNESApp::OnInit() {
	m_mainFrame->Show();
	return true;
}

void LilyNESApp::OnKeyPress(wxKeyEvent& evt) {
	int keyCode = evt.GetKeyCode();
	switch (keyCode) {
	case 'N':
		m_mainFrame->RunUntilNextCycle();
		break;
	case 'M':
		m_mainFrame->RunUntilNextInstruction();
		break;
	case 'H':
		m_mainFrame->RunUntilNextFrame();
		break;
	case 'J':
		m_mainFrame->RunContinuously();
		break;
	case 'Y':
		m_mainFrame->ToggleRefreshRate();
		break;
	case 'U':
		m_mainFrame->SelectNextPalette();
		break;
	}
}