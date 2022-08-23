#pragma once

#include <thread>
#include <chrono>

#include "wx/thread.h"
#include "wxMainFrame.h"
#include "../Environment.h"
#include "../nes/NES.h"
#include "../SoundGenerator.h"


constexpr unsigned int FRAME_INTERVAL_NANOSECONDS = 16666667;
constexpr unsigned int USER_REQUEST_WAIT_TIME_MILLLISECONDS = 100;

constexpr float HIGH_PASS_FILTER_1_CUTOFF = 90;
constexpr float HIGH_PASS_FILTER_2_CUTOFF = 440;
constexpr float LOW_PASS_FILTER_CUTOFF = 14000;
constexpr float AUDIO_FILTERING_DT = 3 * CPU_FREQUENCY;

enum EMULATION_RUNNING_MODE {
	EMULATION_RUNNING_PAUSED,
	EMULATION_RUNNING_USER_CONTROLLED,
	EMULATION_RUNNING_CONTINUOUS_NO_SOUND,
	EMULATION_RUNNING_CONTINUOUS_SOUND,
};

enum EMULATION_USER_REQUEST {
	EMULATION_USER_REQUEST_NONE,
	EMULATION_USER_REQUEST_RETHROW_ILLEGAL_INSTRUCTION_EXCEPTION,
	EMULATION_USER_REQUEST_SOFT_RESET,
	EMULATION_USER_REQUEST_HARD_RESET
};

enum EMULATION_USER_DEBUG_REQUEST {
	EMULATION_USER_DEBUG_REQUEST_NONE,
	EMULATION_USER_DEBUG_REQUEST_NEXT_CYCLE,
	EMULATION_USER_DEBUG_REQUEST_NEXT_INSTRUCTION,
	EMULATION_USER_DEBUG_REQUEST_NEXT_SCANLINE,
	EMULATION_USER_DEBUG_REQUEST_NEXT_FRAME,
};


class wxMainFrame;
class SoundGenerator;
wxDECLARE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);


class wxEmulationThread : public wxThread {
public:
	wxEmulationThread(wxMainFrame* mainFrame, wxSemaphore* exitNotice, Environment* enviroment);
	~wxEmulationThread();
	virtual void* Entry() wxOVERRIDE;
	virtual void OnExit() wxOVERRIDE;
	void LoadROM(const INESFile& romFile);

	EMULATION_RUNNING_MODE GetRunningMode();
	void SetRunningMode(const EMULATION_RUNNING_MODE& runningMode);
	void SetUserRequest(const EMULATION_USER_REQUEST& userRequest);
	void SetUserDebugRequest(const EMULATION_USER_DEBUG_REQUEST& userRequest);
	void RethrowIllegalInstructionException(IllegalInstructionException ex);

	NESState GetCurrentNESState();
	float GetAudioSample();

	bool IsEmulationRunning() const;

private:
	void Setup();
	EMULATION_USER_DEBUG_REQUEST GetUserDebugRequest();

	void PostNESUpdate();

	void RunUntilNextCycle();
	void RunUntilNextInstruction();
	void RunUntilNextScanline();
	void RunUntilNextFrame();
	void EmulationWait();

	void HandleUserRequest();
	void DoPause();
	void DoUserDebugRequestRun();
	void DoContinuousNoSoundRun();
	void DoContinuousSoundRun();

	NES m_nes;
	std::chrono::time_point<std::chrono::steady_clock> m_sleepTargetTime;
	SoundGenerator* m_soundGenerator;
	double m_cyclesRemainingForAudio;
	wxCriticalSection m_audioCritSection;
	bool m_soundModeReady;

	EMULATION_RUNNING_MODE m_runningMode;
	wxCriticalSection m_userRequestCritSection;
	EMULATION_USER_REQUEST m_userRequest;
	wxCriticalSection m_runningModeCritSection;
	wxMessageQueue<EMULATION_USER_DEBUG_REQUEST> m_userDebugRequestQueue;
	NESState m_currentNESState;
	wxCriticalSection m_currentStateCritSection;

	HighPassFilter m_highPassFilter1;
	HighPassFilter m_highPassFilter2;
	LowPassFilter m_lowPassFilter;

	bool m_continuousNoSoundRunInitialized;
	wxMainFrame* m_mainFrame;
	wxSemaphore* m_exitNotice;

	IllegalInstructionException m_illegalEx;
	
};