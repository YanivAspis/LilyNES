#pragma once
#include "wx/wx.h"
#include "wx/msgqueue.h"
#include "../nes/NES.h"
#include "../nes/ROM/INESFile.h"
#include "wxCPUStatePanel.h"
#include "wxRAMStatePanel.h"
#include "Disassembler/wxDisassemblerPanel.h"
#include "wxROMInfoFrame.h"

wxDECLARE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);

class wxDisassemblerPanel;
class wxROMInfoFrame;

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
	void OnLoadROM(wxCommandEvent& evt);
	void OnTestCPU(wxCommandEvent& evt);
	void OnROMInformation(wxCommandEvent& evt);
	void RunUntilNextCycle();
	void RunUntilNextInstruction();

	std::shared_ptr<INESFile> GetLoadedROM() const;

private:
	wxThread::ExitCode Entry();
	void LoadProgram();
	void StopEmulation(bool wait);

	wxMenuBar* m_mainMenuBar;
	wxMenu* m_fileMenu;
	wxMenu* m_viewMenu;

	wxCPUStatePanel* m_cpuStatePanel;
	wxRAMStatePanel* m_ramStatePanel;
	wxDisassemblerPanel* m_disassemblerPanel;
	wxROMInfoFrame* m_ROMInfoFrame;

	NES m_nes;
	NESState m_currNESState;
	std::shared_ptr<INESFile> m_loadedROM;

	wxCriticalSection m_currNESState_cs;
	wxMessageQueue<KeyPressRequestType> m_keypressMessageQueue;


	wxDECLARE_EVENT_TABLE();
};

