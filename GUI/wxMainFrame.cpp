#include "wxMainFrame.h"
#include "wxNESStateEvent.h"
#include <thread>
#include <chrono>

wxDEFINE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);
wxBEGIN_EVENT_TABLE(wxMainFrame, wxFrame)
	EVT_CLOSE(wxMainFrame::OnClose)
wxEND_EVENT_TABLE()

wxMainFrame::wxMainFrame() : wxFrame(nullptr, wxID_ANY, wxString("LilyNES")) {
    Bind(EVT_NES_STATE_THREAD_UPDATE, &wxMainFrame::OnNESStateThreadUpdate, this);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    m_cpuStateFrame = new wxCPUStatePanel(this);
    m_ramStateFrame = new wxRAMStatePanel(this);
    topSizer->Add(m_cpuStateFrame, 1, wxEXPAND);
    topSizer->Add(m_ramStateFrame, 1, wxEXPAND);
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
    while (!GetThread()->TestDestroy())
    {
        m_nes.Clock();
        {
            wxCriticalSectionLocker lock(m_currNESState_cs);
            m_currNESState = m_nes.GetState();
        }
        wxQueueEvent(this, new wxThreadEvent(EVT_NES_STATE_THREAD_UPDATE));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return wxThread::ExitCode(0);
}

void wxMainFrame::OnNESStateThreadUpdate(wxThreadEvent& evt) {
    NESState state;
    {
        wxCriticalSectionLocker lock(m_currNESState_cs);
        state = m_currNESState;
    }
    wxNESStateEvent<CPUState> cpuPostEvt(EVT_CPU_STATE_GUI_UPDATE);
    cpuPostEvt.SetState(state.cpuState);
    wxPostEvent(m_cpuStateFrame, cpuPostEvt);
    wxNESStateEvent<RAMState> ramPostEvt(EVT_RAM_STATE_GUI_UPDATE);
    ramPostEvt.SetState(state.ramState);
    wxPostEvent(m_ramStateFrame, ramPostEvt);
}

void wxMainFrame::OnClose(wxCloseEvent& evt)
{
    if (GetThread() && GetThread()->IsRunning()) {
        GetThread()->Delete();
    }
    Destroy();
}

