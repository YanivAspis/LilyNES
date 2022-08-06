#include "LilyNESApp.h"

wxIMPLEMENT_APP(LilyNESApp);


LilyNESApp::LilyNESApp() {
	m_mainFrame = new wxMainFrame();
	Bind(wxEVT_TIMER, &LilyNESApp::OnTimer, this);
	Bind(wxEVT_CLOSE_WINDOW, &LilyNESApp::OnClose, this);
	Bind(wxEVT_KEY_DOWN, &LilyNESApp::OnKeyDown, this);
	
	// Debugger controls
	m_keyPressFuncs['N'] = &(wxMainFrame::RunUntilNextCycle);
	m_keyPressFuncs['M'] = &(wxMainFrame::RunUntilNextInstruction);
	m_keyPressFuncs['H'] = &(wxMainFrame::RunUntilNextFrame);
	m_keyPressFuncs['J'] = &(wxMainFrame::RunContinuously);
	m_keyPressFuncs['Y'] = &(wxMainFrame::ToggleRefreshRate);
	m_keyPressFuncs['U'] = &(wxMainFrame::SelectNextPalette);


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

bool LilyNESApp::OnInit() {
	m_mainFrame->Show();
	m_keyTimer = new wxTimer(this, wxID_ANY);
	m_keyTimer->Start(30);
	return true;
}

void LilyNESApp::OnClose(wxCloseEvent& evt) {
	m_keyTimer->Stop();
	evt.Skip();
}

void LilyNESApp::OnTimer(wxTimerEvent& evt) {
	if (!m_mainFrame->IsClosing()) {
		for (std::pair<const int, std::pair<NES_CONTROLLER_ID, NES_CONTROLLER_KEY>>& keyPressInfo : m_nesKeyTranslator) {
			if (wxGetKeyState((wxKeyCode)keyPressInfo.first)) {
				m_mainFrame->HandleNESKeyDown(keyPressInfo.second.first, keyPressInfo.second.second);
			}
			else {
				m_mainFrame->HandleNESKeyUp(keyPressInfo.second.first, keyPressInfo.second.second);
			}
		}
	}
	evt.Skip();
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