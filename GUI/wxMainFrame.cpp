#include <thread>
#include <chrono>
#include <string>
#include <fstream>

#include <SDL.h>

#include "wxMainFrame.h"
#include "wxNESStateEvent.h"
#include "../utils/BitwiseUtils.h"
#include "../utils/NESUtils.h"
#include "../nes/ROM/INESFile.h"
#include "../nes/CPU/CPUInstruction.h"

using namespace NESUtils;
using namespace BitwiseUtils;

enum {wxEVT_TIMER_SAVE_PRGRAM = 101, wxEVT_TIMER_REWIND_ADD = 102};


enum MenuOptions {
    // File options
    wxID_LILYNES_OPEN_ROM,
    wxID_LILYNES_EXIT,

    // Emulation options
    wxID_LILYNES_SOFT_RESET,
    wxID_LILYNES_HARD_RESET,

    // Debug options
    wxID_LILYNES_DEBUG_VIEW,
    wxID_LILYNES_VIEW_ROM_INFO,
    wxID_LILYNES_TEST_CPU
};


wxBEGIN_EVENT_TABLE(wxMainFrame, wxFrame)
    EVT_CLOSE(wxMainFrame::OnClose)
    EVT_MENU_OPEN(wxMainFrame::OnMenuOpen)
    EVT_MENU_CLOSE(wxMainFrame::OnMenuClose)
    EVT_ACTIVATE(wxMainFrame::OnWindowActivate)

    EVT_MENU(wxID_LILYNES_OPEN_ROM, wxMainFrame::OnLoadROM)
    EVT_MENU(wxID_LILYNES_EXIT, wxMainFrame::OnMenuExit)

    EVT_MENU(wxID_LILYNES_SOFT_RESET, wxMainFrame::OnSoftReset)
    EVT_MENU(wxID_LILYNES_HARD_RESET, wxMainFrame::OnHardReset)

    EVT_MENU(wxID_LILYNES_DEBUG_VIEW, wxMainFrame::OnDebugView)
    EVT_MENU(wxID_LILYNES_VIEW_ROM_INFO, wxMainFrame::OnROMInformation)
    EVT_MENU(wxID_LILYNES_TEST_CPU, wxMainFrame::OnTestCPU)

    EVT_TIMER(wxEVT_TIMER_SAVE_PRGRAM, wxMainFrame::OnPRGRAMSaveTick)
    EVT_TIMER(wxEVT_TIMER_REWIND_ADD, wxMainFrame::OnRewindAddTick)
wxEND_EVENT_TABLE()


wxMainFrame::wxMainFrame() : wxFrame(nullptr, wxID_ANY, wxString("LilyNES")), m_loadedROM(nullptr), m_debugViewMode(false) {
    SetIcon(wxIcon("sample"));

    Bind(EVT_NES_STATE_THREAD_UPDATE, &wxMainFrame::OnNESStateThreadUpdate, this);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw SDLException("Error opening Video or Audio device.");
    }

    m_mainMenuBar = new wxMenuBar();

    m_fileMenu = new wxMenu();
    m_fileMenu->Append(wxID_LILYNES_OPEN_ROM, wxT("Load ROM..."));
    m_fileMenu->Append(wxID_LILYNES_EXIT, wxT("Exit"));

    m_emulationMenu = new wxMenu();
    m_emulationMenu->Append(wxID_LILYNES_SOFT_RESET, wxT("Soft Reset"));
    m_emulationMenu->Enable(wxID_LILYNES_SOFT_RESET, false);
    m_emulationMenu->Append(wxID_LILYNES_HARD_RESET, wxT("Hard Reset"));
    m_emulationMenu->Enable(wxID_LILYNES_HARD_RESET, false);

    m_debugMenu = new wxMenu();
    
    m_debugMenu->Append(new wxMenuItem(m_debugMenu, wxID_LILYNES_DEBUG_VIEW, wxT("Debug View"), wxT(""), wxITEM_CHECK));
    m_debugMenu->Check(wxID_LILYNES_DEBUG_VIEW, m_debugViewMode);
    m_debugMenu->Append(wxID_LILYNES_VIEW_ROM_INFO, wxT("ROM Information"));
    m_debugMenu->Enable(wxID_LILYNES_VIEW_ROM_INFO, false);
    m_debugMenu->Append(wxID_LILYNES_TEST_CPU, wxT("Test CPU"));


    m_mainMenuBar->Append(m_fileMenu, wxT("File"));
    m_mainMenuBar->Append(m_emulationMenu, wxT("Emulation"));
    m_mainMenuBar->Append(m_debugMenu, wxT("Debug"));

    this->SetMenuBar(m_mainMenuBar);

    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    m_topSizer = new wxBoxSizer(wxHORIZONTAL);
    m_bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    m_displayPanel = new wxDisplayPanel(this);
    m_disassemblerPanel = new wxDisassemblerPanel(this);
    m_cpuStatePanel = new wxCPUStatePanel(this);
    m_ramStatePanel = new wxRAMStatePanel(this);
    m_ppuStatePanel = new wxPPUStatePanel(this);
    m_paletteRAMPanel = new wxPaletteRAMPanel(this);
    m_patternTablePanel = new wxPatternTablePanel(this);
    m_OAMPanel = new wxOAMPanel(this);

    m_topSizer->Add(m_displayPanel, 1, wxEXPAND);
    m_topSizer->Add(m_disassemblerPanel, 1, wxEXPAND);
    m_topSizer->Add(m_cpuStatePanel, 1, wxEXPAND);
    m_topSizer->Add(m_ramStatePanel, 2, wxEXPAND);


    m_bottomSizer->Add(m_ppuStatePanel, 1, wxEXPAND);
    m_bottomSizer->Add(m_paletteRAMPanel, 1, wxEXPAND);
    m_bottomSizer->Add(m_patternTablePanel, 2, wxEXPAND);
    m_bottomSizer->Add(m_OAMPanel, 2, wxEXPAND);

    m_mainSizer->Add(m_topSizer, 3, wxEXPAND);
    m_mainSizer->Add(m_bottomSizer, 2, wxEXPAND);

    this->SetSizer(m_mainSizer);

    m_mainSizer->Show(1, m_debugViewMode);
    for (size_t i = 1; i < m_topSizer->GetItemCount(); i++) {
        m_topSizer->Show(i, m_debugViewMode);
    }

    m_mainSizer->Fit(this);


    m_environment.SetDisplayPanel(m_displayPanel);
    m_ROMInfoFrame = nullptr;
    m_emulationThread = nullptr;
    m_lastRunningMode = EMULATION_RUNNING_PAUSED;
    m_savePRGRAMTimer = new wxTimer(this, wxEVT_TIMER_SAVE_PRGRAM);
    m_rewindAddTimer = new wxTimer(this, wxEVT_TIMER_REWIND_ADD);
    m_closingFlag = false;

    // Sound system initialization
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        wxLogError(wxString::Format(wxT("Error opening Audio device. Details: %s"), SDL_GetError()));
    }
}


void wxMainFrame::StartEmulation()
{
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning()) {
        wxLogError("Emulation already running!");
        return;
    }

    m_emulationThread = new wxEmulationThread(this, &m_emulationThreadExitNotice, &m_environment, REWIND_TAPE_MAX_SIZE);

    if (m_emulationThread->Create() != wxTHREAD_NO_ERROR) {
        m_emulationThread->Delete();
        wxLogError("Error while creating emulation thread");
        return;
    }

    try {
        m_emulationThread->LoadROM(*m_loadedROM);
    }
    catch (UnsupportedMapperException& ex) {
        m_emulationThread->Delete();
        m_emulationThreadExitNotice.Wait();
        wxMessageBox(ex.what());
        m_emulationThread = nullptr;
        return;
    }

    if (m_emulationThread->Run() != wxTHREAD_NO_ERROR) {
        m_emulationThread->Delete();
        m_emulationThreadExitNotice.Wait();
        wxLogError("Failed to run emulation thread.");
        m_emulationThread = nullptr;
        return;
    }

    // Provide a bit of time for thread to set up before running the emulation
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Start periodic saving of battery-backed PRGRAM and rewind states
    m_savePRGRAMTimer->Start(PERIOD_BATTERY_BACKED_RAM_SAVE_MILLISECONDS);
    m_rewindAddTimer->Start(PERIOD_REWIND_SAVE_MILLISECONDS);

    //m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
    //m_emulationThread->SetRunningMode(EMULATION_RUNNING_CONTINUOUS_NO_SOUND);
    m_emulationThread->SetRunningMode(EMULATION_RUNNING_CONTINUOUS_SOUND);

    m_emulationMenu->Enable(wxID_LILYNES_SOFT_RESET, true);
    m_emulationMenu->Enable(wxID_LILYNES_HARD_RESET, true);
}


void wxMainFrame::OnNESStateThreadUpdate(wxThreadEvent& evt) {
    // Perhaps not thread safe, in case event is sent right before emulation ends
    NESState state = m_emulationThread->GetCurrentNESState();

    wxNESStateEvent<CPUState> cpuPostEvt(EVT_CPU_STATE_GUI_UPDATE);
    cpuPostEvt.SetState(state.cpuState);
    wxPostEvent(m_cpuStatePanel, cpuPostEvt);

    wxNESStateEvent<RAMState> ramPostEvt(EVT_RAM_STATE_GUI_UPDATE);
    ramPostEvt.SetState(state.ramState);
    wxPostEvent(m_ramStatePanel, ramPostEvt);

    if (!m_disassemblerPanel->IsInitialized()) {
        wxNESStateEvent<DisassemblerInitializeInfo> disassemblerInitializationEvt(EVT_DISASSEMBLER_INITIALIZE);
        disassemblerInitializationEvt.SetState(DisassemblerInitializeInfo(m_loadedROM->GetPRGROM(), state.cartridgeState.PRGLogicalBanks, state.cartridgeState.PRGBankMapping, state.cpuState.regPC));
        wxPostEvent(m_disassemblerPanel, disassemblerInitializationEvt);
    }
    else if (state.cpuState.instructionFirstCycle) {
        wxNESStateEvent<DisassemblerNextStateInfo> disassemblerNextAddressEvt(EVT_DISASSEMBLER_NEXT_STATE);
        disassemblerNextAddressEvt.SetState(DisassemblerNextStateInfo(state.cpuState.regPC, state.cartridgeState.PRGBankMapping));
        wxPostEvent(m_disassemblerPanel, disassemblerNextAddressEvt);
    }

    wxNESStateEvent<PPUState> ppuPostEvt(EVT_PPU_STATE_GUI_UPDATE);
    ppuPostEvt.SetState(state.ppuState);
    wxPostEvent(m_ppuStatePanel, ppuPostEvt);

    wxNESStateEvent<PaletteRAMState> palettePostEvt(EVT_PALETTE_GUI_UPDATE);
    palettePostEvt.SetState(state.paletteRAMState);
    wxPostEvent(m_paletteRAMPanel, palettePostEvt);

    wxNESStateEvent<PatternPaletteState> patternTablePostEvt(EVT_PATTERN_TABLE_UPDATE);
    PatternPaletteState patternPaletteState;
    patternPaletteState.paletteRAMState = state.paletteRAMState;
    patternPaletteState.patternTableState = state.patternTableState;
    patternTablePostEvt.SetState(patternPaletteState);
    wxPostEvent(m_patternTablePanel, patternTablePostEvt);

    wxNESStateEvent<OAMState> oamPostEvt(EVT_OAM_STATE_GUI_UPDATE);
    oamPostEvt.SetState(state.ppuState.oam);
    wxPostEvent(m_OAMPanel, oamPostEvt);
}

void wxMainFrame::OnLoadROM(wxCommandEvent& evt)
{
    wxFileDialog fileDialog(this, wxT("Select ROM file"), wxEmptyString, wxEmptyString, "NES files (*.nes)|*.nes", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (fileDialog.ShowModal() != wxID_CANCEL) {
        try {
            this->StopEmulation(true);
            m_loadedROM.reset(new INESFile(std::string(fileDialog.GetPath().mb_str())));
            m_debugMenu->Enable(wxID_LILYNES_VIEW_ROM_INFO, true);
            m_disassemblerPanel->Clear();
            this->StartEmulation();
        }
        catch (INESFileException& ex) {
            wxMessageBox(ex.what());
        }
    }
    evt.Skip();
}

void wxMainFrame::OnMenuExit(wxCommandEvent& evt) {
    this->Close();
    evt.Skip();
}


void wxMainFrame::OnSoftReset(wxCommandEvent& evt) {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_SOFT_RESET);
    }
    evt.Skip();
}

void wxMainFrame::OnHardReset(wxCommandEvent& evt) {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_HARD_RESET);
    }
    evt.Skip();
}

void wxMainFrame::OnDebugView(wxCommandEvent& evt) {
    m_debugViewMode = !m_debugViewMode;
    m_debugMenu->Check(wxID_LILYNES_DEBUG_VIEW, m_debugViewMode);

    m_mainSizer->Show(1, m_debugViewMode);
    for (size_t i = 1; i < m_topSizer->GetItemCount(); i++) {
        m_topSizer->Show(i, m_debugViewMode);
    }
    m_mainSizer->Fit(this);

}


void wxMainFrame::OnROMInformation(wxCommandEvent& evt)
{
    m_ROMInfoFrame = new wxROMInfoFrame(this);
    m_ROMInfoFrame->Show();
    evt.Skip();
}

void wxMainFrame::OnTestCPU(wxCommandEvent& evt) {
    wxFileDialog openROMDialog(this, "Open nestest ROM file...", "", "", "NES files (*.nes)|*.nes", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openROMDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    std::string romPath = openROMDialog.GetPath().ToStdString();

    wxFileDialog saveLogDialog(this, "Select location to save log...", "", "", "TXT files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveLogDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    std::string outputPath = saveLogDialog.GetPath().ToStdString();

    RunCPUTest(romPath, outputPath);
    wxMessageBox("Done!");
}

void wxMainFrame::OnMenuOpen(wxMenuEvent& evt) {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_lastRunningMode = m_emulationThread->GetRunningMode();
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_PAUSED);
    }
    evt.Skip();
}

void wxMainFrame::OnMenuClose(wxMenuEvent& evt) {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(m_lastRunningMode);
    }
    evt.Skip();
}

void wxMainFrame::OnWindowActivate(wxActivateEvent& evt) {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        if (evt.GetActive()) {
            m_emulationThread->SetRunningMode(m_lastRunningMode);
        }
        else {
            m_lastRunningMode = m_emulationThread->GetRunningMode();
            m_emulationThread->SetRunningMode(EMULATION_RUNNING_PAUSED);
        }
    }
    evt.Skip();
}

void wxMainFrame::OnClose(wxCloseEvent& evt)
{
    m_closingFlag = true;
    this->StopEmulation(false);
    SDL_Quit();
    Destroy();
}

void wxMainFrame::RunUntilNextCycle() {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
        m_emulationThread->SetUserDebugRequest(EMULATION_USER_DEBUG_REQUEST_NEXT_CYCLE);
    }
}

void wxMainFrame::RunUntilNextInstruction() {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
        m_emulationThread->SetUserDebugRequest(EMULATION_USER_DEBUG_REQUEST_NEXT_INSTRUCTION);
    }
}

void wxMainFrame::RunUntilNextScanline() {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
        m_emulationThread->SetUserDebugRequest(EMULATION_USER_DEBUG_REQUEST_NEXT_SCANLINE);
    }
}

void wxMainFrame::RunUntilNextFrame() {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
        m_emulationThread->SetUserDebugRequest(EMULATION_USER_DEBUG_REQUEST_NEXT_FRAME);
    }
}

void wxMainFrame::RunContinuouslyWithoutSound() {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_CONTINUOUS_NO_SOUND);
    }
}

void wxMainFrame::RunContinuouslyWithSound() {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_CONTINUOUS_SOUND);
    }
}

void wxMainFrame::ToggleRefreshRate() {
    m_displayPanel->ToggleRefreshRate();
}

void wxMainFrame::SelectNextPalette() {
    m_paletteRAMPanel->SelectNextPalette();
    m_patternTablePanel->SelectNextPalette();
}

void wxMainFrame::ClearDisplay() {
    m_displayPanel->ClearDisplay();
}

void wxMainFrame::OnPRGRAMSaveTick(wxTimerEvent& evt) {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->HandlePRGRAMSave();
    }
}

void wxMainFrame::OnRewindAddTick(wxTimerEvent& evt) {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->RewindAddState();
    }
}

void wxMainFrame::QuickSaveState(unsigned int slot) {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetSaveStateSlot(slot);
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_SAVE_STATE);
    }
}

void wxMainFrame::QuickLoadState(unsigned int slot) {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->SetSaveStateSlot(slot);
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_LOAD_STATE);
    }
}

void wxMainFrame::RewindStartCancel() {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        if (m_emulationThread->IsInRewind()) {
            m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_REWIND_CANCEL);
            m_rewindAddTimer->Start();
        }
        else {
            m_rewindAddTimer->Stop();
            m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_REWIND_START);
        }
        
    }
}

void wxMainFrame::RewindLoad() {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning() && m_emulationThread->IsInRewind()) {
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_REWIND_LOAD);
        m_rewindAddTimer->Start();
    }
}

void wxMainFrame::RewindBack() {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning() && m_emulationThread->IsInRewind()) {
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_REWIND_BACK);
    }
}

void wxMainFrame::RewindForward() {
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning() && m_emulationThread->IsInRewind()) {
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_REWIND_FORWARD);
    }
}


bool wxMainFrame::IsClosing() const {
    return m_closingFlag;
}

void wxMainFrame::StopEmulation(bool wait = false) {
    m_savePRGRAMTimer->Stop();
    if (m_emulationThread != nullptr && m_emulationThread->IsEmulationRunning() && m_emulationThread->IsRunning()) {
        m_emulationThread->Delete();
        if (wait) {
            m_emulationThreadExitNotice.Wait();
        }
        m_emulationThread = nullptr;
    }
}

std::shared_ptr<INESFile> wxMainFrame::GetLoadedROM() const {
    return m_loadedROM;
}
