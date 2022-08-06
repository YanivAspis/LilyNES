#include <thread>
#include <chrono>
#include <string>
#include <fstream>

#include "wxMainFrame.h"
#include "wxNESStateEvent.h"
#include "../utils/BitwiseUtils.h"
#include "../utils/NESUtils.h"
#include "../nes/ROM/INESFile.h"
#include "../nes/CPU/CPUInstruction.h"

using namespace NESUtils;
using namespace BitwiseUtils;


enum MenuOptions {
    wxID_OPEN_ROM,
    wxID_TEST_CPU,
    wxID_VIEW_ROM_INFO
};


wxBEGIN_EVENT_TABLE(wxMainFrame, wxFrame)
    EVT_CLOSE(wxMainFrame::OnClose)
    EVT_MENU(wxID_OPEN_ROM, wxMainFrame::OnLoadROM)
    EVT_MENU(wxID_TEST_CPU, wxMainFrame::OnTestCPU)
    EVT_MENU(wxID_VIEW_ROM_INFO, wxMainFrame::OnROMInformation)
wxEND_EVENT_TABLE()


wxMainFrame::wxMainFrame() : wxFrame(nullptr, wxID_ANY, wxString("LilyNES")), m_loadedROM(nullptr) {
    Bind(EVT_NES_STATE_THREAD_UPDATE, &wxMainFrame::OnNESStateThreadUpdate, this);

    m_mainMenuBar = new wxMenuBar();
    m_fileMenu = new wxMenu();
    m_fileMenu->Append(wxID_OPEN_ROM, wxT("Load ROM..."));
    m_fileMenu->Append(wxID_TEST_CPU, wxT("Test CPU"));
    m_viewMenu = new wxMenu();
    m_viewMenu->Append(wxID_VIEW_ROM_INFO, wxT("ROM Information"));
    m_viewMenu->Enable(wxID_VIEW_ROM_INFO, false);

    m_mainMenuBar->Append(m_fileMenu, wxT("File"));
    m_mainMenuBar->Append(m_viewMenu, wxT("View"));

    this->SetMenuBar(m_mainMenuBar);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    m_displayPanel = new wxDisplayPanel(this);
    m_disassemblerPanel = new wxDisassemblerPanel(this);
    m_cpuStatePanel = new wxCPUStatePanel(this);
    m_ramStatePanel = new wxRAMStatePanel(this);
    m_ppuStatePanel = new wxPPUStatePanel(this);
    m_paletteRAMPanel = new wxPaletteRAMPanel(this);
    m_patternTablePanel = new wxPatternTablePanel(this);
    m_OAMPanel = new wxOAMPanel(this);

    topSizer->Add(m_displayPanel, 1, wxSHAPED);
    topSizer->Add(m_disassemblerPanel, 1, wxEXPAND);
    topSizer->Add(m_cpuStatePanel, 1, wxEXPAND);
    topSizer->Add(m_ramStatePanel, 2, wxEXPAND);

    bottomSizer->Add(m_ppuStatePanel, 1, wxEXPAND);
    bottomSizer->Add(m_paletteRAMPanel, 1, wxEXPAND);
    bottomSizer->Add(m_patternTablePanel, 2, wxEXPAND);
    bottomSizer->Add(m_OAMPanel, 2, wxEXPAND);

    mainSizer->Add(topSizer, 3, wxEXPAND);
    mainSizer->Add(bottomSizer, 2, wxEXPAND);
    
    this->SetSizer(mainSizer);
    mainSizer->Fit(this);

    m_environment.SetDisplayPanel(m_displayPanel);
    m_ROMInfoFrame = nullptr;
    m_emulationThread = nullptr;
    m_closingFlag = false;
}


void wxMainFrame::StartEmulation()
{
    if (m_emulationThread != nullptr) {
        wxLogError("Emulation already running!");
        return;
    }

    m_emulationThread = new wxEmulationThread(this, &m_emulationThreadExitNotice, &m_environment);

    if (m_emulationThread->Create() != wxTHREAD_NO_ERROR) {
        m_emulationThread->Delete();
        wxLogError("Error while creating emulation thread");
        return;
    }

    try {
        m_emulationThread->LoadROM(*m_loadedROM);
    }
    catch (UnsupportedMapperException ex) {
        m_emulationThread->Delete();
        wxMessageBox(ex.what());
        return;
    }

    m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
    //m_emulationThread->SetRunningMode(EMULATION_RUNNING_CONTINUOUS);

    if (m_emulationThread->Run() != wxTHREAD_NO_ERROR) {
        m_emulationThread->Delete();
        wxLogError("Failed to run emulation thread.");
        return;
    }
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
        disassemblerInitializationEvt.SetState(DisassemblerInitializeInfo(m_loadedROM->GetPRGROM(), 0x8000, 0xFFFF, state.cpuState.regPC));
        wxPostEvent(m_disassemblerPanel, disassemblerInitializationEvt);
    }
    else if (state.cpuState.instructionFirstCycle) {
        wxNESStateEvent<uint16_t> disassemblerNextAddressEvt(EVT_DISASSEMBLER_NEXT_ADDRESS);
        disassemblerNextAddressEvt.SetState(state.cpuState.currInstruction.instructionAddress);
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

void wxMainFrame::OnClose(wxCloseEvent& evt)
{
    m_closingFlag = true;
    this->StopEmulation(false);
    Destroy();
}

void wxMainFrame::OnLoadROM(wxCommandEvent& evt)
{
    wxFileDialog fileDialog(this, wxT("Select ROM file"), wxEmptyString, wxEmptyString, "NES files (*.nes)|*.nes", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (fileDialog.ShowModal() != wxID_CANCEL) {
        try {
            this->StopEmulation(true);
            m_loadedROM.reset(new INESFile(std::string(fileDialog.GetPath().mb_str())));
            m_viewMenu->Enable(wxID_VIEW_ROM_INFO, true);
            m_disassemblerPanel->Clear();
            this->StartEmulation();
        }
        catch (INESFileException& ex) {
            wxMessageBox(ex.what());
        }
    }
    evt.Skip();
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

void wxMainFrame::RunUntilNextCycle() {
    // Perhaps not thread safe
    if (m_emulationThread != nullptr && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_NEXT_CYCLE);
    }
}

void wxMainFrame::RunUntilNextInstruction() {
    // Perhaps not thread safen
    if (m_emulationThread != nullptr && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_NEXT_INSTRUCTION);
    }
}

void wxMainFrame::RunUntilNextFrame() {
    // Perhaps not thread safen
    if (m_emulationThread != nullptr && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_NEXT_FRAME);
    }
}
void wxMainFrame::RunContinuously() {
    // Perhaps not thread safen
    if (m_emulationThread != nullptr && m_emulationThread->IsRunning()) {
        m_emulationThread->SetRunningMode(EMULATION_RUNNING_CONTINUOUS);
    }
}

void wxMainFrame::ToggleRefreshRate() {
    m_displayPanel->ToggleRefreshRate();
}

void wxMainFrame::SelectNextPalette() {
    m_paletteRAMPanel->SelectNextPalette();
    m_patternTablePanel->SelectNextPalette();
}

void wxMainFrame::HandleNESKeyDown(NES_CONTROLLER_ID controller, NES_CONTROLLER_KEY key) {
    m_environment.HandleNESKeyDown(controller, key);
}

void wxMainFrame::HandleNESKeyUp(NES_CONTROLLER_ID controller, NES_CONTROLLER_KEY key) {
    m_environment.HandleNESKeyUp(controller, key);
}

bool wxMainFrame::IsClosing() const {
    return m_closingFlag;
}

void wxMainFrame::StopEmulation(bool wait = false) {
    if (m_emulationThread != nullptr && m_emulationThread->IsRunning()) {
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
