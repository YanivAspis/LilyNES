#include "wxEmulationThread.h"
#include "wx/wx.h"

constexpr double NUM_CYCLES_PER_AUDIO_SAMPLE = 3 * (double)CPU_FREQUENCY / SOUND_SAMPLE_RATE;

std::atomic<bool> s_emulationRunning;

wxDEFINE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);

wxEmulationThread::wxEmulationThread(wxMainFrame* mainFrame, wxSemaphore* exitNotice, Environment* environment, size_t rewindTapeMaxSize) : m_environment(environment), m_nes(environment), 
		m_rewindTape(rewindTapeMaxSize), m_illegalEx(0, 0), m_highPassFilter1(HIGH_PASS_FILTER_1_CUTOFF, AUDIO_FILTERING_DT), m_highPassFilter2(HIGH_PASS_FILTER_2_CUTOFF, AUDIO_FILTERING_DT), 
		m_lowPassFilter(LOW_PASS_FILTER_CUTOFF, AUDIO_FILTERING_DT) {

	m_continuousNoSoundRunInitialized = false;
	m_runningMode = EMULATION_RUNNING_PAUSED;
	m_userRequest = EMULATION_USER_REQUEST_NONE;
	m_mainFrame = mainFrame;
	m_exitNotice = exitNotice;
	s_emulationRunning = false;

	m_soundGenerator = new SoundGenerator(this);
	m_cyclesRemainingForAudio = NUM_CYCLES_PER_AUDIO_SAMPLE;

	m_loadedROMChecksum.fill(0);
	m_saveStateSlot = 0;
	m_saveStateValid.fill(false);

	m_rewindStartFlag = false;
	m_rewindPreviousRunningMode = EMULATION_RUNNING_PAUSED;
	m_inRewind = false;
	m_rewindAddFlag = false;
}

wxEmulationThread::~wxEmulationThread() {
	delete m_soundGenerator;
	m_soundGenerator = nullptr;
}

void* wxEmulationThread::Entry(){
	s_emulationRunning = true;
	this->Setup();
	this->PostNESUpdate();
	while (!this->TestDestroy()) {
		try {
			EMULATION_RUNNING_MODE running_mode = this->GetRunningMode();
			if (running_mode != EMULATION_RUNNING_CONTINUOUS_SOUND)
			{
				this->HandleEmulationEvents();
				this->HandleUserRequest();
				this->HandleRewindFlags();
			}
			switch (running_mode) {
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
	if (m_nes.PRGRAMNeedsSaving()) {
		SaveBatteryBackedRAM(m_loadedROMChecksum, m_nes.GetPRGRAM());
	}
	s_emulationRunning = false;
	m_soundGenerator->DisableSound();
	m_mainFrame->ClearDisplay();
	m_exitNotice->Post();
}

void wxEmulationThread::LoadROM(const INESFile& romFile) {
	m_nes.LoadROM(romFile);
	m_loadedROMChecksum = romFile.GetChecksum();
	if (romFile.GetHeader().IsPRGRAMBatteryBacked()) {
		std::vector<uint8_t> PRGRAMContent;
		if (LoadBatteryBackedRAM(m_loadedROMChecksum, PRGRAMContent)) {
			m_nes.LoadPRGRAM(PRGRAMContent);
		}
	}
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
		m_highPassFilter1.Restart();
		m_highPassFilter2.Restart();
		m_lowPassFilter.Restart();
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

void wxEmulationThread::SetSaveStateSlot(unsigned int slot) {
	assert(slot >= 0 && slot < 8);
	m_saveStateSlot = slot;
}

void wxEmulationThread::RethrowIllegalInstructionException(IllegalInstructionException ex) {
	m_illegalEx = ex;
	this->PostEmulationEvent(EMULATION_EVENT_RETHROW_ILLEGAL_INSTRUCTION_EXCEPTION);
}

void wxEmulationThread::HandlePRGRAMSave() {
	this->PostEmulationEvent(EMULATION_EVENT_SAVE_PRGRAM);
}

NESState wxEmulationThread::GetCurrentNESState() {
	{
		wxCriticalSectionLocker enter(m_currentStateCritSection);
		return m_currentNESState;
	}
}

float wxEmulationThread::GetAudioSample() {
	float sampleSum = 0;
	unsigned int samplesCollected = 0;
	float sample = 0;
	wxCriticalSectionLocker enter(m_audioCritSection);
	while (m_cyclesRemainingForAudio > 0.5) {
		m_nes.Clock();
		sample = m_nes.GetAudioSample();
		m_cyclesRemainingForAudio--;
		sample = m_highPassFilter1.Filter(sample);
		//sample = m_highPassFilter2.Filter(sample);
		sample = m_lowPassFilter.Filter(sample);
		sampleSum += sample;
		samplesCollected++;
	}
	m_cyclesRemainingForAudio += NUM_CYCLES_PER_AUDIO_SAMPLE;
	this->HandleEmulationEvents();
	this->HandleUserRequest();
	this->HandleRewindFlags();
	return sampleSum / samplesCollected;
}

bool wxEmulationThread::IsEmulationRunning() const {
	return s_emulationRunning;
}

bool wxEmulationThread::IsInRewind() {
	wxCriticalSectionLocker enter(m_inRewindCritSection);
	return m_inRewind;
}

void wxEmulationThread::RewindAddState() {

	wxCriticalSectionLocker enter(m_rewindAddFlagCritSection);
	m_rewindAddFlag = true;
}

void wxEmulationThread::Setup() {
	m_nes.HardReset();
	m_saveStateValid.fill(false);
	for (unsigned int slot : GetSaveStateSlotsAvailable(m_loadedROMChecksum)) {
		m_saveStateValid[slot] = true;
	}
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

void wxEmulationThread::PostEmulationEvent(EMULATION_EVENT evt) {
	wxCriticalSectionLocker enter(m_pendingEmulationEventsCritSection);
	m_pendingEmulationEvents.push(evt);
}

void wxEmulationThread::HandleEmulationEvents() {
	wxCriticalSectionLocker enter(m_pendingEmulationEventsCritSection);
	while (!m_pendingEmulationEvents.empty()) {
		EMULATION_EVENT evt;
		evt = m_pendingEmulationEvents.front();
		m_pendingEmulationEvents.pop();
		switch (evt) {
		case EMULATION_EVENT_RETHROW_ILLEGAL_INSTRUCTION_EXCEPTION:
			throw m_illegalEx;
			break;
		case EMULATION_EVENT_SAVE_PRGRAM:
			if (m_nes.PRGRAMNeedsSaving()) {
				SaveBatteryBackedRAM(m_mainFrame->GetLoadedROM()->GetChecksum(), m_nes.GetPRGRAM());
			}
			break;
		}
	}
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
		if (m_runningMode == EMULATION_RUNNING_CONTINUOUS_SOUND) {
			m_soundGenerator->DisableSound();
		}
		m_nes.SoftReset();
		if (m_runningMode == EMULATION_RUNNING_CONTINUOUS_SOUND) {
			m_highPassFilter1.Restart();
			m_highPassFilter2.Restart();
			m_lowPassFilter.Restart();
			m_soundGenerator->EnableSound();
		}
		break;
	case EMULATION_USER_REQUEST_HARD_RESET:
		if (m_runningMode == EMULATION_RUNNING_CONTINUOUS_SOUND) {
			m_soundGenerator->DisableSound();
		}
		m_nes.HardReset();
		if (m_runningMode == EMULATION_RUNNING_CONTINUOUS_SOUND) {
			m_highPassFilter1.Restart();
			m_highPassFilter2.Restart();
			m_lowPassFilter.Restart();
			m_soundGenerator->EnableSound();
		}
		break;
	case EMULATION_USER_REQUEST_SAVE_STATE:
	{
		NESState state = m_nes.GetState();
		QuickSaveStateToFile(m_loadedROMChecksum, m_saveStateSlot, state);
		m_saveStateValid[m_saveStateSlot] = true;
		break;
	}
	case EMULATION_USER_REQUEST_LOAD_STATE:
		if (m_saveStateValid[m_saveStateSlot]) {
			NESState state = QuickLoadStateFromFile(m_loadedROMChecksum, m_saveStateSlot);
			m_nes.LoadState(state);	
		}
		break;
	case EMULATION_USER_REQUEST_REWIND_START:
		m_rewindStartFlag = true;
		break;
	case EMULATION_USER_REQUEST_REWIND_CANCEL:
		m_rewindTape.RewindCancel();
		this->SetRunningMode(m_rewindPreviousRunningMode);
		{
			wxCriticalSectionLocker enter(m_inRewindCritSection);
			m_inRewind = false;
		}
		m_environment->ClearDisplayMessage();
		break;
	case EMULATION_USER_REQUEST_REWIND_LOAD:
	{
		NESState state = m_rewindTape.RewindLoad();
		m_nes.LoadState(state);
		this->SetRunningMode(m_rewindPreviousRunningMode);
		{
			wxCriticalSectionLocker enter(m_inRewindCritSection);
			m_inRewind = false;
		}
		m_environment->ClearDisplayMessage();
		break;
	}
	case EMULATION_USER_REQUEST_REWIND_BACK:
		m_environment->UpdateDisplay(m_rewindTape.RewindBack());
		break;
	case EMULATION_USER_REQUEST_REWIND_FORWARD:
		m_environment->UpdateDisplay(m_rewindTape.RewindForward());
		break;
	}
}

void wxEmulationThread::HandleRewindFlags() {
	{
		wxCriticalSectionLocker enter(m_rewindAddFlagCritSection);
		if (m_rewindAddFlag && m_nes.IsPictureReady()) {
			m_rewindAddFlag = false;
			m_rewindTape.RewindAddState(m_nes.GetState(), m_nes.GetCurrentPicture());
		}
	}
	if (m_rewindStartFlag && m_nes.IsPictureReady()) {
		m_rewindStartFlag = false;
		m_rewindTape.RewindStart(m_nes.GetState(), m_nes.GetCurrentPicture());
		{
			wxCriticalSectionLocker enter(m_runningModeCritSection);
			m_rewindPreviousRunningMode = m_runningMode;
		}
		this->SetRunningMode(EMULATION_RUNNING_PAUSED);
		{
			wxCriticalSectionLocker enter(m_inRewindCritSection);
			m_inRewind = true;
		}
		m_environment->SetDisplayMessage("REWIND", -1);
	}
}


void wxEmulationThread::DoPause() {
	if (!m_continuousNoSoundRunInitialized) {
		m_sleepTargetTime = std::chrono::steady_clock::now();
		m_continuousNoSoundRunInitialized = false;
	}
	m_sleepTargetTime += std::chrono::nanoseconds(FRAME_INTERVAL_NANOSECONDS);
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



