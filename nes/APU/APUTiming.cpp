#include "APU2A03.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

void APU2A03::Clock() {
	if (m_frameCounterWriteCycles > 0) {
		m_frameCounterWriteCycles--;
		if (m_frameCounterWriteCycles == 0) {
			// Frame counter is reset and possibly generate quarter and half frame signals
			m_frameCounter = 0;
			if (m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_5_STEP) {
				this->DoFrameCounterHalf();
			}
		}
	}

	if (m_frameCounter % 2 == 0) {
		m_pulse1.ClockTimer();
		m_pulse2.ClockTimer();
	}

	m_triangle.ClockTimer();
	m_noise.ClockTimer();
	m_dmc.ClockTimer();

	if (m_frameCounter == APU_FRAME_COUNTER_QUARTER) {
		this->DoFrameCounterQuarter();
	}
	else if (m_frameCounter == APU_FRAME_COUNTER_HALF) {
		this->DoFrameCounterHalf();
	}
	else if (m_frameCounter == APU_FRAME_COUNTER_THREE_QUARTERS) {
		this->DoFrameCounterThreeQuarters();
	}
	else if (m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_4_STEP && m_frameCounter == APU_FRAME_COUNTER_4_STEP_LAST_FRAME) {
		this->DoFrameCounterMode4Last();
	}
	else if (m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_5_STEP && m_frameCounter == APU_FRAME_COUNTER_5_STEP_LAST_FRAME) {
		this->DoFrameCounterMode5Last();
	}

	this->IncrementFrameCounter();
}

void APU2A03::DoFrameCounterQuarter() {
	m_pulse1.ClockEnvelope();
	m_pulse2.ClockEnvelope();
	m_triangle.ClockLinearCounter();
	m_noise.ClockEnvelope();
}

void APU2A03::DoFrameCounterHalf() {
	m_pulse1.ClockEnvelope();
	m_pulse2.ClockEnvelope();
	m_triangle.ClockLinearCounter();
	m_noise.ClockEnvelope();

	m_pulse1.ClockLengthCounter();
	m_pulse2.ClockLengthCounter();
	m_triangle.ClockLengthCounter();
	m_noise.ClockLengthCounter();

	m_pulse1.ClockSweep();
	m_pulse2.ClockSweep();
}

void APU2A03::DoFrameCounterThreeQuarters() {
	m_pulse1.ClockEnvelope();
	m_pulse2.ClockEnvelope();
	m_triangle.ClockLinearCounter();
	m_noise.ClockEnvelope();
}

void APU2A03::DoFrameCounterMode4Last() {
	this->GenerateFrameInterrupt();

	m_pulse1.ClockEnvelope();
	m_pulse2.ClockEnvelope();
	m_triangle.ClockLinearCounter();
	m_noise.ClockEnvelope();

	m_pulse1.ClockLengthCounter();
	m_pulse2.ClockLengthCounter();
	m_triangle.ClockLengthCounter();
	m_noise.ClockLengthCounter();

	m_pulse1.ClockSweep();
	m_pulse2.ClockSweep();
}

void APU2A03::DoFrameCounterMode5Last() {
	m_pulse1.ClockEnvelope();
	m_pulse2.ClockEnvelope();
	m_triangle.ClockLinearCounter();
	m_noise.ClockEnvelope();

	m_pulse1.ClockLengthCounter();
	m_pulse2.ClockLengthCounter();
	m_triangle.ClockLengthCounter();
	m_noise.ClockLengthCounter();

	m_pulse1.ClockSweep();
	m_pulse2.ClockSweep();
}

void APU2A03::IncrementFrameCounter() {
	m_frameCounter++;
	if ((m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_5_STEP && m_frameCounter == APU_FRAME_COUNTER_5_STEP_MAX) ||
		(m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_4_STEP && m_frameCounter == APU_FRAME_COUNTER_4_STEP_MAX)) {
		m_frameCounter = 0;
	}
}

void APU2A03::GenerateFrameInterrupt() {
	if (!m_frameCounterRegister.flags.irqInhibit) {
		m_cpu->RaiseIRQ(APU_FRAME_IRQ_ID);
		m_statusRegister.flags.frameInterrupt = 1;
	}
}
