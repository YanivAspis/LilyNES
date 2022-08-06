#pragma once
#include "wx/wx.h"
#include "wx/msgqueue.h"
#include "../nes/ROM/INESFile.h"
#include "wxCPUStatePanel.h"
#include "wxRAMStatePanel.h"
#include "Disassembler/wxDisassemblerPanel.h"
#include "wxROMInfoFrame.h"
#include "wxDisplayPanel.h"
#include "wxPPUStatePanel.h"
#include "wxPaletteRAMPanel.h"
#include "wxPatternTablePanel.h"
#include "wxOAMPanel.h"
#include "wxEmulationThread.h"
#include "../Environment.h"

class wxEmulationThread;
class wxROMInfoFrame;

class wxMainFrame : public wxFrame
{
public:
	wxMainFrame();

	void StartEmulation();
	void OnNESStateThreadUpdate(wxThreadEvent& evt);
	void OnClose(wxCloseEvent& evt);
	void OnLoadROM(wxCommandEvent& evt);
	void OnTestCPU(wxCommandEvent& evt);
	void OnROMInformation(wxCommandEvent& evt);

	std::shared_ptr<INESFile> GetLoadedROM() const;

	void RunUntilNextCycle();
	void RunUntilNextInstruction();
	void RunUntilNextFrame();
	void RunContinuously();
	void ToggleRefreshRate();
	void SelectNextPalette();

	void HandleNESKeyDown(NES_CONTROLLER_ID controller, NES_CONTROLLER_KEY key);
	void HandleNESKeyUp(NES_CONTROLLER_ID controller, NES_CONTROLLER_KEY key);

	bool IsClosing() const;
	

private:
	void StopEmulation(bool wait);

	Environment m_environment;

	wxEmulationThread* m_emulationThread;
	wxSemaphore m_emulationThreadExitNotice;

	wxMenuBar* m_mainMenuBar;
	wxMenu* m_fileMenu;
	wxMenu* m_viewMenu;

	wxDisplayPanel* m_displayPanel;
	wxCPUStatePanel* m_cpuStatePanel;
	wxRAMStatePanel* m_ramStatePanel;
	wxDisassemblerPanel* m_disassemblerPanel;
	wxPPUStatePanel* m_ppuStatePanel;
	wxPaletteRAMPanel* m_paletteRAMPanel;
	wxPatternTablePanel* m_patternTablePanel;
	wxOAMPanel* m_OAMPanel;
	wxROMInfoFrame* m_ROMInfoFrame;
	

	std::shared_ptr<INESFile> m_loadedROM;

	wxCriticalSection m_currNESState_cs;

	bool m_closingFlag;

	wxDECLARE_EVENT_TABLE();
};

