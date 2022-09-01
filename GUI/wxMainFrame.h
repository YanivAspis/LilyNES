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

constexpr int PERIOD_BATTERY_BACKED_RAM_SAVE_MILLISECONDS = 60000;

class wxEmulationThread;
enum EMULATION_RUNNING_MODE;
class wxROMInfoFrame;

class wxMainFrame : public wxFrame
{
public:
	wxMainFrame();

	void StartEmulation();
	void OnNESStateThreadUpdate(wxThreadEvent& evt);

	void OnLoadROM(wxCommandEvent& evt);
	void OnMenuExit(wxCommandEvent& evt);

	void OnSoftReset(wxCommandEvent& evt);
	void OnHardReset(wxCommandEvent& evt);

	void OnDebugView(wxCommandEvent& evt);
	void OnROMInformation(wxCommandEvent& evt);
	void OnTestCPU(wxCommandEvent& evt);
	
	void OnMenuOpen(wxMenuEvent& evt);
	void OnMenuClose(wxMenuEvent& evt);
	void OnWindowActivate(wxActivateEvent& evt);
	void OnClose(wxCloseEvent& evt);

	std::shared_ptr<INESFile> GetLoadedROM() const;

	void RunUntilNextCycle();
	void RunUntilNextInstruction();
	void RunUntilNextScanline();
	void RunUntilNextFrame();
	void RunContinuouslyWithoutSound();
	void RunContinuouslyWithSound();
	void ToggleRefreshRate();
	void SelectNextPalette();
	void ClearDisplay();
	void OnPRGRAMSaveTick(wxTimerEvent& evt);
	void SaveState();
	void LoadState();


	bool IsClosing() const;
	

private:
	void StopEmulation(bool wait);

	Environment m_environment;

	wxEmulationThread* m_emulationThread;
	wxSemaphore m_emulationThreadExitNotice;
	EMULATION_RUNNING_MODE m_lastRunningMode;

	wxBoxSizer* m_mainSizer;
	wxBoxSizer* m_topSizer;
	wxBoxSizer* m_bottomSizer;

	wxMenuBar* m_mainMenuBar;
	wxMenu* m_fileMenu;
	wxMenu* m_emulationMenu;
	wxMenu* m_debugMenu;

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
	wxTimer* m_savePRGRAMTimer;

	bool m_debugViewMode;
	bool m_closingFlag;

	wxDECLARE_EVENT_TABLE();
};

