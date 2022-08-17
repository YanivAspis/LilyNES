#include "APU2A03.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

void APU2A03::Clock() {
	if (m_frameCounter % 2 == 0) {
		this->APUClock();
	}

	if (m_frameCounter == 0) {
		this->DoFrameCounterZero();
	}
	else if (m_frameCounter == APU_FRAME_COUNTER_QUARTER) {
		this->DoFrameCounterQuarter();
	}
	else if (m_frameCounter == APU_FRAME_COUNTER_HALF) {
		this->DoFrameCounterHalf();
	}
	else if (m_frameCounter == APU_FRAME_COUNTER_THREE_QUARTERS) {
		this->DoFrameCounterThreeQuarters();
	}
	else if (m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_4_STEP && m_frameCounter == APU_FRAME_COUNTER_4_STEP_PENULTIMATE_FRAME) {
		this->DoFrameCounterMode4Penultimate();
	}
	else if (!m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_4_STEP && m_frameCounter == APU_FRAME_COUNTER_4_STEP_LAST_FRAME) {
		this->DoFrameCounterMode4Last();
	}
	else if (m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_5_STEP && m_frameCounter == APU_FRAME_COUNTER_5_STEP_LAST_FRAME) {
		this->DoFrameCounterMode5Last();
	}

	this->IncrementFrameCounter();
}

void APU2A03::APUClock() {
	// Clock Pulse 1 Timer
	// Clock Pulse 2 Timer
	// Clock Noise Timer
}

void APU2A03::DoFrameCounterZero() {
	if (m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_4_STEP) {
		this->GenerateFrameInterrupt();
	}
}

void APU2A03::DoFrameCounterOne() {
	//m_frameIRQSent = false;
}

void APU2A03::DoFrameCounterQuarter() {
	// Clock Pulse 1 Envelope
	// Clock Pulse 2 Envelope
	// Clock Triangle linear counter
	// Clock Noise Envelope
}

void APU2A03::DoFrameCounterHalf() {
	// Clock Pulse 1 Envelope
	// Clock Pulse 2 Envelope
	// Clock Triangle linear counter
	// Clock Noise Envelope

	// Clock Pulse 1 Length Counter
	// Clock Pulse 2 Length Counter
	// Clock Triangle Length Counter
	// Clock Noise Length Counter

	// Clock Pulse 1 Sweep
	// Clock Pulse 2 Sweep
}

void APU2A03::DoFrameCounterThreeQuarters() {
	// Clock Pulse 1 Envelope
	// Clock Pulse 2 Envelope
	// Clock Triangle linear counter
	// Clock Noise Envelope
}

void APU2A03::DoFrameCounterMode4Penultimate() {
	this->GenerateFrameInterrupt();
}

void APU2A03::DoFrameCounterMode4Last() {
	this->GenerateFrameInterrupt();

	// Clock Pulse 1 Envelope
	// Clock Pulse 2 Envelope
	// Clock Triangle linear counter
	// Clock Noise Envelope

	// Clock Pulse 1 Length Counter
	// Clock Pulse 2 Length Counter
	// Clock Triangle Length Counter
	// Clock Noise Length Counter

	// Clock Pulse 1 Sweep
	// Clock Pulse 2 Sweep
}

void APU2A03::DoFrameCounterMode5Last() {
	// Clock Pulse 1 Envelope
	// Clock Pulse 2 Envelope
	// Clock Triangle linear counter
	// Clock Noise Envelope

	// Clock Pulse 1 Length Counter
	// Clock Pulse 2 Length Counter
	// Clock Triangle Length Counter
	// Clock Noise Length Counter

	// Clock Pulse 1 Sweep
	// Clock Pulse 2 Sweep
}

void APU2A03::IncrementFrameCounter() {
	m_frameCounter++;
	if ((m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_5_STEP && m_frameCounter == APU_FRAME_COUNTER_5_STEP_MAX) ||
		(m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_4_STEP && m_frameCounter == APU_FRAME_COUNTER_4_STEP_MAX)) {
		m_frameCounter = 0;
	}
}

void APU2A03::GenerateFrameInterrupt() {
	if (!m_frameCounterRegister.flags.irqInhibit && !m_frameIRQSent) {
		m_cpu->RaiseIRQ();
		m_frameIRQSent = true;
	}
}
