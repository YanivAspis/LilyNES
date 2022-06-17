#include "wxMainFrame.h"
#include "wxNESStateEvent.h"
#include "../nes/BitwiseUtils.h"
#include <thread>
#include <chrono>
#include <string>

using namespace BitwiseUtils;


wxDEFINE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);
wxBEGIN_EVENT_TABLE(wxMainFrame, wxFrame)
    EVT_CLOSE(wxMainFrame::OnClose)
wxEND_EVENT_TABLE()


wxMainFrame::wxMainFrame() : wxFrame(nullptr, wxID_ANY, wxString("LilyNES")) {
    Bind(EVT_NES_STATE_THREAD_UPDATE, &wxMainFrame::OnNESStateThreadUpdate, this);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    m_disassemblerPanel = new wxDisassemblerPanel(this);
    m_cpuStatePanel = new wxCPUStatePanel(this);
    m_ramStatePanel = new wxRAMStatePanel(this);
    topSizer->Add(m_disassemblerPanel, 1, wxEXPAND);
    topSizer->Add(m_cpuStatePanel, 1, wxEXPAND);
    topSizer->Add(m_ramStatePanel, 1, wxEXPAND);
    this->SetSizer(topSizer);
    topSizer->Fit(this);
}


void wxMainFrame::StartEmulation()
{
    if (CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
    {
        wxLogError("Failed to create emulation thread.");
        return;
    }
    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError("Failed to run emulation thread.");
        return;
    }
}


wxThread::ExitCode wxMainFrame::Entry()
{
    m_nes.HardReset();
    this->LoadProgram();
    KeyPressRequestType keyPressRequest = REQUEST_NONE;
    {
        wxCriticalSectionLocker lock(m_currNESState_cs);
        m_currNESState = m_nes.GetState();
    }
    wxQueueEvent(this, new wxThreadEvent(EVT_NES_STATE_THREAD_UPDATE));
    while (!GetThread()->TestDestroy())
    {
        m_keypressMessageQueue.ReceiveTimeout(100, keyPressRequest);
        int cycles_to_run = 0;
        switch (keyPressRequest) {
            case REQUEST_NONE:
                continue;
            case REQUEST_NEXT_CYCLE:
                cycles_to_run = 1;
                break;
            case REQUEST_NEXT_INSTRUCTION:
                {
                    wxCriticalSectionLocker lock(m_currNESState_cs);
                    cycles_to_run = m_currNESState.cpuState.cyclesRemaining + 1;
                }
                break;
        }
        keyPressRequest = REQUEST_NONE;
        try {
            while (cycles_to_run > 0) {
                m_nes.Clock();
                cycles_to_run--;
            }
            {
                wxCriticalSectionLocker lock(m_currNESState_cs);
                m_currNESState = m_nes.GetState();
            }
            wxQueueEvent(this, new wxThreadEvent(EVT_NES_STATE_THREAD_UPDATE));
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        catch (IllegalInstructionException ex) {
            wxMessageBox("Illegal instruction detected. Killing emulation.");
            return wxThread::ExitCode(1);
        }
    }
    return wxThread::ExitCode(0);
}

void wxMainFrame::LoadProgram() {
    std::string program_str = "A2 08 CA 8E 00 02 E0 03 D0 F8 8E 01 02 00";

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
}

void wxMainFrame::OnNESStateThreadUpdate(wxThreadEvent& evt) {
    NESState state;
    {
        wxCriticalSectionLocker lock(m_currNESState_cs);
        state = m_currNESState;
    }
    wxNESStateEvent<CPUState> cpuPostEvt(EVT_CPU_STATE_GUI_UPDATE);
    cpuPostEvt.SetState(state.cpuState);
    wxPostEvent(m_cpuStatePanel, cpuPostEvt);
    wxNESStateEvent<RAMState> ramPostEvt(EVT_RAM_STATE_GUI_UPDATE);
    ramPostEvt.SetState(state.ramState);
    wxPostEvent(m_ramStatePanel, ramPostEvt);
    if (!m_disassemblerPanel->IsInitialized()) {
        wxNESStateEvent<DisassemblerInitializeInfo> disassemblerInitializationEvt(EVT_DISASSEMBLER_INITIALIZE);
        disassemblerInitializationEvt.SetState(DisassemblerInitializeInfo(state.ramState.content, 0x600, 0x6FF, state.cpuState.regPC));
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
    if (GetThread() && GetThread()->IsRunning()) {
        GetThread()->Delete();
    }
    Destroy();
}

void wxMainFrame::RunUntilNextCycle() {
    m_keypressMessageQueue.Post(REQUEST_NEXT_CYCLE);
}

void wxMainFrame::RunUntilNextInstruction() {
    m_keypressMessageQueue.Post(REQUEST_NEXT_INSTRUCTION);
}