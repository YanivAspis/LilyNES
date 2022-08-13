#include "wxEmulationThread.h"
#include "wx/wx.h"

wxDEFINE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);

wxEmulationThread::wxEmulationThread(wxMainFrame* mainFrame, wxSemaphore* exitNotice, Environment* enviroment) : m_nes(enviroment) {
	m_continuousRunInitialized = false;
	m_runningMode = EMULATION_RUNNING_PAUSED;
	m_userRequest = EMULATION_USER_REQUEST_NONE;
	m_mainFrame = mainFrame;
	m_exitNotice = exitNotice;
}

void* wxEmulationThread::Entry(){
	this->Setup();
	this->PostNESUpdate();
	while (!this->TestDestroy()) {
		this->HandleUserRequest();
		try {
			switch (this->GetRunningMode()) {
			case EMULATION_RUNNING_PAUSED:
				this->DoPause();
				break;
			case EMULATION_RUNNING_USER_CONTROLLED:
				this->DoUserDebugRequestRun();
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
	wxCriticalSectionLocker enter(m_userRequestCritSection);
	m_userRequest = userRequest;
}


void wxEmulationThread::SetUserDebugRequest(const EMULATION_USER_DEBUG_REQUEST& userRequest) {
	m_userDebugRequestQueue.Post(userRequest);
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

EMULATION_USER_DEBUG_REQUEST wxEmulationThread::GetUserDebugRequest() {
	EMULATION_USER_DEBUG_REQUEST request = EMULATION_USER_DEBUG_REQUEST_NONE;
	m_userDebugRequestQueue.ReceiveTimeout(USER_REQUEST_WAIT_TIME_MILLLISECONDS, request);
	return request;
}

void wxEmulationThread::RunUntilNextCycle() {
	m_nes.RunUntilNextCycle();
}

void wxEmulationThread::RunUntilNextInstruction() {
	m_nes.RunUntilNextInstruction();
}

void wxEmulationThread::RunUntilNextScanline() {
	m_nes.RunUntilNextScanline();
}

void wxEmulationThread::RunUntilNextFrame() {
	m_nes.RunUntilNextFrame();
}

void wxEmulationThread::EmulationWait() {
	std::this_thread::sleep_until(m_sleepTargetTime);
}

void wxEmulationThread::HandleUserRequest() {
	EMULATION_USER_REQUEST userRequest;
	{
		wxCriticalSectionLocker enter(m_userRequestCritSection);
		userRequest = m_userRequest;
		m_userRequest = EMULATION_USER_REQUEST_NONE;
	}
	switch (userRequest) {
	case EMULATION_USER_REQUEST_SOFT_RESET:
		m_nes.SoftReset();
		break;
	case EMULATION_USER_REQUEST_HARD_RESET:
		m_nes.HardReset();
		break;
	}
}

void wxEmulationThread::DoPause() {
	m_continuousRunInitialized = false;
	this->EmulationWait();
}

void wxEmulationThread::DoUserDebugRequestRun() {
	m_continuousRunInitialized = false;
	switch (this->GetUserDebugRequest()) {
	case EMULATION_USER_DEBUG_REQUEST_NONE:
		break;
	case EMULATION_USER_DEBUG_REQUEST_NEXT_CYCLE:
		this->RunUntilNextCycle();
		this->PostNESUpdate();
		break;
	case EMULATION_USER_DEBUG_REQUEST_NEXT_INSTRUCTION:
		this->RunUntilNextInstruction();
		this->PostNESUpdate();
		break;
	case EMULATION_USER_DEBUG_REQUEST_NEXT_SCANLINE:
		this->RunUntilNextScanline();
		this->PostNESUpdate();
		break;
	case EMULATION_USER_DEBUG_REQUEST_NEXT_FRAME:
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
	m_sleepTargetTime += std::chrono::nanoseconds(FRAME_INTERVAL_NANOSECONDS);
	this->RunUntilNextFrame();
	this->EmulationWait();
}

