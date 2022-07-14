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
	if (keyCode == 'M') {
		m_mainFrame->RunUntilNextCycle();
	}
	else if (keyCode == 'N') {
		m_mainFrame->RunUntilNextInstruction();
	}
}