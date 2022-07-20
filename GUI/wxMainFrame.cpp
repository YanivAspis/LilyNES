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

    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    m_disassemblerPanel = new wxDisassemblerPanel(this);
    m_cpuStatePanel = new wxCPUStatePanel(this);
    m_ramStatePanel = new wxRAMStatePanel(this);
    topSizer->Add(m_disassemblerPanel, 1, wxEXPAND);
    topSizer->Add(m_cpuStatePanel, 1, wxEXPAND);
    topSizer->Add(m_ramStatePanel, 1, wxEXPAND);
    this->SetSizer(topSizer);
    topSizer->Fit(this);

    m_ROMInfoFrame = nullptr;
    m_emulationThread = nullptr;
}


void wxMainFrame::StartEmulation()
{
    if (m_emulationThread != nullptr) {
        wxLogError("Emulation already running!");
        return;
    }

    m_emulationThread = new wxEmulationThread(this, &m_emulationThreadExitNotice);
    //m_emulationThread->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
    m_emulationThread->SetRunningMode(EMULATION_RUNNING_CONTINUOUS);
    try {
        m_emulationThread->LoadROM(*m_loadedROM);
    }
    catch (UnsupportedMapperException ex) {
        m_emulationThread->Delete();
        wxMessageBox(ex.what());
        return;
    }
    if (m_emulationThread->Create() != wxTHREAD_NO_ERROR) {
        m_emulationThread->Delete();
        wxLogError("Error while creating emulation thread");
        return;
    }
    if (m_emulationThread->Run() != wxTHREAD_NO_ERROR) {
        m_emulationThread->Delete();
        wxLogError("Failed to run emulation thread.");
        return;
    }
}


/*
void wxMainFrame::LoadProgram() {
    // Snake program
    //std::string program_str = "20 06 06 20 38 06 20 0d 06 20 2a 06 60 a9 02 85 02 a9 04 85 03 a9 11 85 10 a9 10 85 12 a9 0f 85 14 a9 04 85 11 85 13 85 15 60 a5 fe 85 00 a5 fe 29 03 18 69 02 85 01 60 20 4d 06 20 8d 06 20 c3 06 20 19 07 20 20 07 20 2d 07 4c 38 06 a5 ff c9 77 f0 0d c9 64 f0 14 c9 73 f0 1b c9 61 f0 22 60 a9 04 24 02 d0 26 a9 01 85 02 60 a9 08 24 02 d0 1b a9 02 85 02 60 a9 01 24 02 d0 10 a9 04 85 02 60 a9 02 24 02 d0 05 a9 08 85 02 60 60 20 94 06 20 a8 06 60 a5 00 c5 10 d0 0d a5 01 c5 11 d0 07 e6 03 e6 03 20 2a 06 60 a2 02 b5 10 c5 10 d0 06 b5 11 c5 11 f0 09 e8 e8 e4 03 f0 06 4c aa 06 4c 35 07 60 a6 03 ca 8a b5 10 95 12 ca 10 f9 a5 02 4a b0 09 4a b0 19 4a b0 1f 4a b0 2f a5 10 38 e9 20 85 10 90 01 60 c6 11 a9 01 c5 11 f0 28 60 e6 10 a9 1f 24 10 f0 1f 60 a5 10 18 69 20 85 10 b0 01 60 e6 11 a9 06 c5 11 f0 0c 60 c6 10 a5 10 29 1f c9 1f f0 01 60 4c 35 07 a0 00 a5 fe 91 00 60 a6 03 a9 00 81 10 a2 00 a9 01 81 10 60 a2 00 ea ea ca d0 fb 60";

    // Multiplication program
    //std::string program_str = "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";

    // Day of week program
    std::string program_str = "A9 04 A2 07 A0 7A E0 03 B0 01 88 49 7F C0 C8 7D 26 06 85 00 98 20 22 06 E5 00 85 00 98 4A 4A 18 65 00 69 07 90 FC 60 01 05 06 03 01 05 03 00 04 02 06 04";

    // split str by space
    std::vector<std::string> program_hex_lst;
    size_t pos;
    while ((pos = program_str.find(' ')) != std::string::npos) {
        program_hex_lst.push_back(program_str.substr(0, pos));
        program_str.erase(0, pos+1);
    }
    program_hex_lst.push_back(program_str);

    // convert str vector to byte vector
    std::vector<uint8_t> program_hex;
    for (auto const& hex_str : program_hex_lst) {
        unsigned long m = std::stoul(hex_str, 0, 16);
        program_hex.push_back(m);
    }

    // load to ram
    NESState state = m_nes.GetState();
    uint16_t pc = state.cpuState.regPC;
    for (uint16_t i = 0; i < (uint16_t)program_hex.size(); i++) {
        state.ramState.content[Add16Bit(pc,i)] = program_hex[i];
    }
    m_nes.LoadState(state);
}*/

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
        disassemblerNextAddressEvt.SetState(state.cpuState.currInstructionAddress);
        wxPostEvent(m_disassemblerPanel, disassemblerNextAddressEvt);
    }
}

void wxMainFrame::OnClose(wxCloseEvent& evt)
{
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
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_NEXT_CYCLE);
    }
}

void wxMainFrame::RunUntilNextInstruction() {
    // Perhaps not thread safen
    if (m_emulationThread != nullptr && m_emulationThread->IsRunning()) {
        m_emulationThread->SetUserRequest(EMULATION_USER_REQUEST_NEXT_INSTRUCTION);
    }
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
