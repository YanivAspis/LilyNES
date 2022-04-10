#include "LilyNESApp.h"

wxIMPLEMENT_APP(LilyNESApp);


LilyNESApp::LilyNESApp() {
	m_mainFrame = new wxMainFrame();
}

bool LilyNESApp::OnInit() {
	m_mainFrame->Show();
	m_mainFrame->StartEmulation();
	return true;
}