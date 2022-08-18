#include "APUPulse.h"
#include "LengthCounter.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

APUPulseState::APUPulseState() {
	parameters.value = 0;
	sweep.value = 0;
	timerLow = 0;
	timerHighLengthCounter.value = 0;

	timer = 0;
	lengthCounter = 0;
	waveformIndex = 0;
	
	silenced = true;
}

APUPulse::APUPulse(PulseSweepBehaviour sweepBehaviour)
{
	m_sweepBehaviour = sweepBehaviour;

	m_parameters.value = 0;
	m_sweep.value = 0;
	m_timerLow = 0;
	m_timerHighLengthCounter.value = 0;

	m_timer = 0;
	m_lengthCounter = 0;
	m_waveformIndex = 0;

	m_silenced = true;
}

void APUPulse::SoftReset()
{
	// Not clear if these should be reset
	m_parameters.value = 0;
	m_sweep.value = 0;
	m_timerLow = 0;
	m_timerHighLengthCounter.value = 0;

	m_timer = 0;
	m_lengthCounter = 0;
	m_waveformIndex = 0;

	m_silenced = true;
}

void APUPulse::HardReset()
{
	m_parameters.value = 0;
	m_sweep.value = 0;
	m_timerLow = 0;
	m_timerHighLengthCounter.value = 0;

	m_timer = 0;
	m_lengthCounter = 0;
	m_waveformIndex = 0;

	m_silenced = true;
}

void APUPulse::ClockTimer()
{
	if (m_timer == 0) {
		this->ClockWaveformIndex();
		m_timer = this->GetTimerReload();
	}
	else {
		m_timer--;
	}
}

void APUPulse::ClockEnvelope()
{
}

void APUPulse::ClockSweep()
{
}

void APUPulse::ClockLengthCounter()
{
	if (!m_parameters.flags.lengthCounterHalt && m_lengthCounter > 0) {
		m_lengthCounter--;
	}
}

uint8_t APUPulse::GetAudioSample()
{
	if (m_lengthCounter == 0 || this->GetTimerReload() < APU_PULSE_MIN_TIMER_VALUE) {
		return 0;
	}
	else {
		// Assume constant volume for now
		return m_parameters.flags.volumePeriod * TestBit8(APU_PULSE_DUTYCYCLE_TO_WAVEFORM[m_parameters.flags.dutyCycle], m_waveformIndex);
	}
}

void APUPulse::SilenceChannel()
{
	m_silenced = true;
	m_lengthCounter = 0;
}

void APUPulse::PlayChannel()
{
	m_silenced = false;
}

void APUPulse::WriteParameters(uint8_t data)
{
	m_parameters.value = data;
}

void APUPulse::WriteSweep(uint8_t data)
{
	m_sweep.value = data;
}

void APUPulse::WriteTimerLow(uint8_t data)
{
	// Should I reset something?
	m_timerLow = data;
}

void APUPulse::WriteTimerHighLengthCounter(uint8_t data)
{
	m_timerHighLengthCounter.value = data;
	if (!m_silenced) {
		m_lengthCounter = APU_LENGTH_COUNTER_VALUES[m_timerHighLengthCounter.flags.lengthCounterLoad];
	}
	
	m_waveformIndex = 0;
	// Reset envelope
}

APUPulseState APUPulse::GetState() const
{
	APUPulseState state;
	state.parameters.value = m_parameters.value;
	state.sweep.value = m_sweep.value;
	state.timerLow = m_timerLow;
	state.timerHighLengthCounter.value = m_timerHighLengthCounter.value;

	state.timer = m_timer;
	state.lengthCounter = m_lengthCounter;
	state.waveformIndex = m_waveformIndex;

	state.silenced = m_silenced;

	return state;
}

void APUPulse::LoadState(APUPulseState& state)
{
	m_parameters.value = state.parameters.value;
	m_sweep.value = state.sweep.value;
	m_timerLow = state.timerLow;
	m_timerHighLengthCounter.value = state.timerHighLengthCounter.value;

	m_timer = state.timer;
	m_lengthCounter = state.lengthCounter;
	m_waveformIndex = state.waveformIndex;

	m_silenced = state.silenced;
}

void APUPulse::ClockWaveformIndex()
{
	if (m_waveformIndex == 0) {
		m_waveformIndex = APU_PULSE_WAVEFORM_LENGTH - 1;
	}
	else {
		m_waveformIndex--;
	}
}

uint16_t APUPulse::GetTimerReload()
{
	return CombineBytes(m_timerLow, m_timerHighLengthCounter.flags.timerHigh);
}
