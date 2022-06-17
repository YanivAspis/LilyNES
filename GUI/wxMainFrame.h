#pragma once
#include "wx/wx.h"
#include "wx/msgqueue.h"
#include "../nes/NES.h"
#include "wxCPUStatePanel.h"
#include "wxRAMStatePanel.h"
#include "wxDisassemblerPanel.h"

wxDECLARE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);

class wxDisassemblerPanel;

enum KeyPressRequestType {
	REQUEST_NONE,
	REQUEST_NEXT_CYCLE,
	REQUEST_NEXT_INSTRUCTION
};

class wxMainFrame : public wxFrame, public wxThreadHelper
{
public:
	wxMainFrame();

	void StartEmulation();
	void OnNESStateThreadUpdate(wxThreadEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void RunUntilNextCycle();
	void RunUntilNextInstruction();

private:
	wxThread::ExitCode Entry();
	void LoadProgram();

	wxCPUStatePanel* m_cpuStatePanel;
	wxRAMStatePanel* m_ramStatePanel;
	wxDisassemblerPanel* m_disassemblerPanel;

	NES m_nes;
	NESState m_currNESState;
	wxCriticalSection m_currNESState_cs;
	wxMessageQueue<KeyPressRequestType> m_keypressMessageQueue;

	wxDECLARE_EVENT_TABLE();
};

