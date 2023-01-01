#include "LilyNESApp.h"

wxIMPLEMENT_APP(LilyNESApp);


LilyNESApp::LilyNESApp() {
	m_mainFrame = new wxMainFrame();
	Bind(wxEVT_KEY_DOWN, &LilyNESApp::OnKeyDown, this);
	Bind(wxEVT_KEY_UP, &LilyNESApp::OnKeyUp, this);
	
	// Debugger controls
	m_keyDownFuncs['N'] = &(wxMainFrame::RunUntilNextCycle);
	m_keyDownFuncs['M'] = &(wxMainFrame::RunUntilNextInstruction);
	m_keyDownFuncs['H'] = &(wxMainFrame::RunUntilNextScanline);
	m_keyDownFuncs['J'] = &(wxMainFrame::RunUntilNextFrame);
	m_keyDownFuncs['K'] = &(wxMainFrame::RunContinuouslyWithoutSound);
	m_keyDownFuncs['L'] = &(wxMainFrame::RunContinuouslyWithSound);
	m_keyDownFuncs['Y'] = &(wxMainFrame::ToggleRefreshRate);
	m_keyDownFuncs['U'] = &(wxMainFrame::SelectNextPalette);
	m_keyDownFuncs['R'] = &(wxMainFrame::RewindStartCancel);
	m_keyDownFuncs['Q'] = &(wxMainFrame::RewindLoad);
	m_keyDownFuncs['W'] = &(wxMainFrame::RewindBack);
	m_keyDownFuncs['E'] = &(wxMainFrame::RewindForward);

	
	// User controls
	//m_keyUpFuncs['P'] = &(wxMainFrame::SaveState);
	//m_keyUpFuncs['R'] = &(wxMainFrame::LoadState);
}

bool LilyNESApp::OnInit() {
	m_mainFrame->Show();
	m_mainFrame->ClearDisplay();
	return true;
}

void LilyNESApp::OnKeyDown(wxKeyEvent& evt) {
	int keyCode = evt.GetKeyCode();

	std::map < int, std::function<void(wxMainFrame&)>>::iterator keyPressIterator = m_keyDownFuncs.find(keyCode);
	if (keyPressIterator != m_keyDownFuncs.end()) {
		keyPressIterator->second(*m_mainFrame);
	}
	evt.Skip();
}

void LilyNESApp::OnKeyUp(wxKeyEvent& evt) {
	int keyCode = evt.GetKeyCode();

	// Check if key request is for quick save/load state
	if (keyCode >= '1' && keyCode <= '8') {
		if (evt.ControlDown()) {
			m_mainFrame->QuickSaveState(keyCode - '1');
		}
		else {
			m_mainFrame->QuickLoadState(keyCode - '1');
		}
		evt.Skip();
		return;
	}

	// All other user requests
	std::map < int, std::function<void(wxMainFrame&)>>::iterator keyPressIterator = m_keyUpFuncs.find(keyCode);
	if (keyPressIterator != m_keyUpFuncs.end()) {
		keyPressIterator->second(*m_mainFrame);
	}
	evt.Skip();
}