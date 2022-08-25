#include "LilyNESApp.h"

wxIMPLEMENT_APP(LilyNESApp);


LilyNESApp::LilyNESApp() {
	m_mainFrame = new wxMainFrame();
	Bind(wxEVT_KEY_DOWN, &LilyNESApp::OnKeyDown, this);
	
	// Debugger controls
	m_keyPressFuncs['N'] = &(wxMainFrame::RunUntilNextCycle);
	m_keyPressFuncs['M'] = &(wxMainFrame::RunUntilNextInstruction);
	m_keyPressFuncs['H'] = &(wxMainFrame::RunUntilNextScanline);
	m_keyPressFuncs['J'] = &(wxMainFrame::RunUntilNextFrame);
	m_keyPressFuncs['K'] = &(wxMainFrame::RunContinuouslyWithoutSound);
	m_keyPressFuncs['L'] = &(wxMainFrame::RunContinuouslyWithSound);
	m_keyPressFuncs['Y'] = &(wxMainFrame::ToggleRefreshRate);
	m_keyPressFuncs['U'] = &(wxMainFrame::SelectNextPalette);
}

bool LilyNESApp::OnInit() {
	m_mainFrame->Show();
	m_mainFrame->ClearDisplay();
	return true;
}

void LilyNESApp::OnKeyDown(wxKeyEvent& evt) {
	int keyCode = evt.GetKeyCode();

	std::map < int, std::function<void(wxMainFrame&)>>::iterator keyPressIterator = m_keyPressFuncs.find(keyCode);
	if (keyPressIterator != m_keyPressFuncs.end()) {
		keyPressIterator->second(*m_mainFrame);
		evt.Skip();
		return;
	}
	evt.Skip();
}