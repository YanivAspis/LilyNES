#include "wxEmulationThread.h"
#include "wx/wx.h"

constexpr double NUM_CYCLES_PER_AUDIO_SAMPLE = 3 * (double)CPU_FREQUENCY / SOUND_SAMPLE_RATE;


wxDEFINE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);

wxEmulationThread::wxEmulationThread(wxMainFrame* mainFrame, wxSemaphore* exitNotice, Environment* enviroment) : m_nes(enviroment) {
	m_continuousNoSoundRunInitialized = false;
	m_runningMode = EMULATION_RUNNING_PAUSED;
	m_userRequest = EMULATION_USER_REQUEST_NONE;
	m_mainFrame = mainFrame;
	m_exitNotice = exitNotice;

	m_soundGenerator = new SoundGenerator(this);
	m_cyclesRemainingForAudio = NUM_CYCLES_PER_AUDIO_SAMPLE;
}

wxEmulationThread::~wxEmulationThread() {
	delete m_soundGenerator;
	m_soundGenerator = nullptr;
}

void* wxEmulationThread::Entry(){
	this->Setup();
	this->PostNESUpdate();
	while (!this->TestDestroy()) {
		{
			wxCriticalSectionLocker enter(m_audioCritSection);
			this->HandleUserRequest();
		}
		try {
			switch (this->GetRunningMode()) {
			case EMULATION_RUNNING_PAUSED:
				this->DoPause();
				break;
			case EMULATION_RUNNING_USER_CONTROLLED:
				this->DoUserDebugRequestRun();
				break;
			case EMULATION_RUNNING_CONTINUOUS_NO_SOUND:
				this->DoContinuousNoSoundRun();
				break;
			case EMULATION_RUNNING_CONTINUOUS_SOUND:
				this->DoContinuousSoundRun();
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
	m_soundGenerator->DisableSound();
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
	if (m_runningMode == EMULATION_RUNNING_CONTINUOUS_SOUND) {
		m_cyclesRemainingForAudio = NUM_CYCLES_PER_AUDIO_SAMPLE;
		m_soundGenerator->EnableSound();
	}
	else {
		m_soundGenerator->DisableSound();
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

float wxEmulationThread::GetAudioSample() {
	static double globalTime = 0;
	{
		wxCriticalSectionLocker enter(m_audioCritSection);
		while (m_cyclesRemainingForAudio > 0.5) {
			m_nes.Clock();
			m_cyclesRemainingForAudio--;
		}
		m_cyclesRemainingForAudio += NUM_CYCLES_PER_AUDIO_SAMPLE;
	}

	float sample = 1.0 * sin(2 * M_PI * 440.0 * globalTime);
	globalTime += 1.0 / 44100;
	return sample;
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
		wxCriticalSectionLocker enter(m_audioCritSection);
		m_nes.RunUntilNextCycle();
}

void wxEmulationThread::RunUntilNextInstruction() {

	wxCriticalSectionLocker enter(m_audioCritSection);
	m_nes.RunUntilNextInstruction();
}

void wxEmulationThread::RunUntilNextScanline() {
	wxCriticalSectionLocker enter(m_audioCritSection);
	m_nes.RunUntilNextScanline();
}

void wxEmulationThread::RunUntilNextFrame() {
	wxCriticalSectionLocker enter(m_audioCritSection);
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
	m_continuousNoSoundRunInitialized = false;
	this->EmulationWait();
}

void wxEmulationThread::DoUserDebugRequestRun() {
	m_continuousNoSoundRunInitialized = false;
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

void wxEmulationThread::DoContinuousNoSoundRun() {
	if (!m_continuousNoSoundRunInitialized) {
		m_sleepTargetTime = std::chrono::steady_clock::now();
		m_continuousNoSoundRunInitialized = true;
	}
	m_sleepTargetTime += std::chrono::nanoseconds(FRAME_INTERVAL_NANOSECONDS);
	this->RunUntilNextFrame();
	this->EmulationWait();
}

void wxEmulationThread::DoContinuousSoundRun() {
	m_continuousNoSoundRunInitialized = false;
}



