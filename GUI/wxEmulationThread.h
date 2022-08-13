#pragma once

#include <thread>
#include <chrono>

#include "wx/thread.h"
#include "wxMainFrame.h"
#include "../Environment.h"
#include "../nes/NES.h"



constexpr unsigned int FRAME_INTERVAL_NANOSECONDS = 16666667;
constexpr unsigned int USER_REQUEST_WAIT_TIME_MILLLISECONDS = 100;

enum EMULATION_RUNNING_MODE {
	EMULATION_RUNNING_PAUSED,
	EMULATION_RUNNING_USER_CONTROLLED,
	EMULATION_RUNNING_CONTINUOUS
};

enum EMULATION_USER_REQUEST {
	EMULATION_USER_REQUEST_NONE,
	EMULATION_USER_REQUEST_NEXT_CYCLE,
	EMULATION_USER_REQUEST_NEXT_INSTRUCTION,
	EMULATION_USER_REQUEST_NEXT_SCANLINE,
	EMULATION_USER_REQUEST_NEXT_FRAME
};


class wxMainFrame;
wxDECLARE_EVENT(EVT_NES_STATE_THREAD_UPDATE, wxThreadEvent);


class wxEmulationThread : public wxThread {
public:
	wxEmulationThread(wxMainFrame* mainFrame, wxSemaphore* exitNotice, Environment* enviroment);
	virtual void* Entry() wxOVERRIDE;
	virtual void OnExit() wxOVERRIDE;
	void LoadROM(const INESFile& romFile);

	EMULATION_RUNNING_MODE GetRunningMode();
	void SetRunningMode(const EMULATION_RUNNING_MODE& runningMode);
	void SetUserRequest(const EMULATION_USER_REQUEST& userRequest);

	NESState GetCurrentNESState();

private:
	void Setup();
	EMULATION_USER_REQUEST GetUserRequest();

	void PostNESUpdate();

	void RunUntilNextCycle();
	void RunUntilNextInstruction();
	void RunUntilNextScanline();
	void RunUntilNextFrame();
	void EmulationWait();

	void DoPause();
	void DoUserRequestRun();
	void DoContinuousRun();

	NES m_nes;
	std::chrono::time_point<std::chrono::steady_clock> m_sleepTargetTime;

	EMULATION_RUNNING_MODE m_runningMode;
	wxCriticalSection m_runningModeCritSection;
	wxMessageQueue<EMULATION_USER_REQUEST> m_userRequestQueue;
	NESState m_currentNESState;
	wxCriticalSection m_currentStateCritSection;

	bool m_continuousRunInitialized;
	wxMainFrame* m_mainFrame;
	wxSemaphore* m_exitNotice;
};