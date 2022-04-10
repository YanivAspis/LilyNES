#pragma once
#include "wx/wx.h"
#include "../nes/NES.h"
#include "wxCPUStatePanel.h"
#include "wxRAMStatePanel.h"

wxDECLARE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);


class wxMainFrame : public wxFrame, public wxThreadHelper
{
public:
	wxMainFrame();

	void StartEmulation();
	void OnNESStateThreadUpdate(wxThreadEvent& evt);
	void OnClose(wxCloseEvent& evt);

private:
	wxThread::ExitCode Entry();

	wxCPUStatePanel* m_cpuStateFrame;
	wxRAMStatePanel* m_ramStateFrame;

	NES m_nes;
	NESState m_currNESState;
	wxCriticalSection m_currNESState_cs;

	wxDECLARE_EVENT_TABLE();
};

