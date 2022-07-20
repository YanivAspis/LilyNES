#include "wxEmulationThread.h"
#include "wx/wx.h"

wxDEFINE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);

wxEmulationThread::wxEmulationThread(wxMainFrame* mainFrame, wxSemaphore* exitNotice) {
	m_continuousRunInitialized = false;
	m_runningMode = EMULATION_RUNNING_PAUSED;
	m_mainFrame = mainFrame;
	m_exitNotice = exitNotice;
}

void* wxEmulationThread::Entry(){
	this->Setup();
	this->PostNESUpdate();
	while (!this->TestDestroy()) {
		try {
			/*
			if (m_nes.ProbeCPUState().irqPending > 0) {
				this->SetRunningMode(EMULATION_RUNNING_USER_CONTROLLED);
			}*/
			switch (this->GetRunningMode()) {
			case EMULATION_RUNNING_PAUSED:
				this->DoPause();
				break;
			case EMULATION_RUNNING_USER_CONTROLLED:
				this->DoUserRequestRun();
				break;
			case EMULATION_RUNNING_CONTINUOUS:
				this->DoContinuousRun();
				break;
			}
		}
		catch (IllegalInstructionException ex) {
			wxMessageBox(ex.what());
			return wxThread::ExitCode(1);
		}
	}
	return wxThread::ExitCode(0);
}

void wxEmulationThread::OnExit() {
	m_exitNotice->Post();
}

void wxEmulationThread::LoadROM(const INESFile& romFile) {
	m_nes.LoadROM(romFile);
}

EMULATION_RUNNING_MODE wxEmulationThread::GetRunningMode() {
	EMULATION_RUNNING_MODE mode;
	{
		wxCriticalSectionLocker enter(m_runningModeCritSection);
		mode = m_runningMode;
	}
	return mode;
}

void wxEmulationThread::SetRunningMode(const EMULATION_RUNNING_MODE& runningMode) {
	{
		wxCriticalSectionLocker enter(m_runningModeCritSection);
		m_runningMode = runningMode;
	}
	if (m_runningMode == EMULATION_RUNNING_USER_CONTROLLED) {
		this->PostNESUpdate();
	}
}

void wxEmulationThread::SetUserRequest(const EMULATION_USER_REQUEST& userRequest) {
	{
		wxCriticalSectionLocker enter(m_userRequestCritSection);
		m_userRequestQueue.Post(userRequest);
	}
}

NESState wxEmulationThread::GetCurrentNESState() {
	{
		wxCriticalSectionLocker enter(m_currentStateCritSection);
		return m_currentNESState;
	}
}


void wxEmulationThread::Setup() {
	m_nes.HardReset();
}

void wxEmulationThread::PostNESUpdate() {
	{
		wxCriticalSectionLocker enter(m_currentStateCritSection);
		m_currentNESState = m_nes.GetState();
	}
	wxQueueEvent(m_mainFrame, new wxThreadEvent(EVT_NES_STATE_THREAD_UPDATE));
}

EMULATION_USER_REQUEST wxEmulationThread::GetUserRequest() {
	EMULATION_USER_REQUEST request = EMULATION_USER_REQUEST_NONE;
	{
		wxCriticalSectionLocker enter(m_userRequestCritSection);
		m_userRequestQueue.ReceiveTimeout(100, request);
	}
	return request;
}

void wxEmulationThread::RunUntilNextCycle() {
	m_nes.RunUntilNextCycle();
}

void wxEmulationThread::RunUntilNextInstruction() {
	m_nes.RunUntilNextInstruction();
}

void wxEmulationThread::RunUntilNextFrame() {
	m_nes.RunUntilNextFrame();
}

void wxEmulationThread::EmulationWait() {
	std::this_thread::sleep_until(m_sleepTargetTime);
}

void wxEmulationThread::DoPause() {
	m_continuousRunInitialized = false;
	this->EmulationWait();
}

void wxEmulationThread::DoUserRequestRun() {
	m_continuousRunInitialized = false;
	switch (this->GetUserRequest()) {
	case EMULATION_USER_REQUEST_NONE:
		break;
	case EMULATION_USER_REQUEST_NEXT_CYCLE:
		this->RunUntilNextCycle();
		this->PostNESUpdate();
		break;
	case EMULATION_USER_REQUEST_NEXT_INSTRUCTION:
		this->RunUntilNextInstruction();
		this->PostNESUpdate();
		break;
	case EMULATION_USER_REQUEST_NEXT_FRAME:
		this->RunUntilNextFrame();
		this->PostNESUpdate();
		break;
	}
}

void wxEmulationThread::DoContinuousRun() {
	if (!m_continuousRunInitialized) {
		m_sleepTargetTime = std::chrono::steady_clock::now();
		m_continuousRunInitialized = true;
	}
	//m_sleepTargetTime += std::chrono::nanoseconds(FRAME_INTERVAL_NANOSECONDS);
	//this->RunUntilNextFrame();
	m_sleepTargetTime += std::chrono::nanoseconds(3000);
	this->RunUntilNextCycle();
	this->EmulationWait();
}

