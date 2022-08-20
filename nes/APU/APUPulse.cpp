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
	sweepDivider = 0;
	sweepReloadFlag = false;
	
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
	m_sweepDivider = 0;
	m_sweepReloadFlag = false;

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
	m_envelope.SoftReset();
	m_sweepDivider = 0;
	m_sweepReloadFlag = false;

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
	m_envelope.HardReset();
	m_sweepDivider = 0;
	m_sweepReloadFlag = false;

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
	m_envelope.Clock();
}

void APUPulse::ClockSweep()
{
	if (m_sweepDivider == 0 && m_sweep.flags.enabled && !this->IsSweepMuting()) {
		uint16_t targetPeriod = this->GetSweepTargetPeriod();
		m_timerLow = ExtractLowByte(targetPeriod);
		m_timerHighLengthCounter.flags.timerHigh = ExtractHighByte(targetPeriod);		
	}
	if (m_sweepDivider == 0 || m_sweepReloadFlag) {
		m_sweepReloadFlag = false;
		m_sweepDivider = m_sweep.flags.period;
	}
	else {
		m_sweepDivider--;
	}
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
	else if (m_parameters.flags.constantVolume) {
		return m_parameters.flags.volumePeriod * TestBit8(APU_PULSE_DUTYCYCLE_TO_WAVEFORM[m_parameters.flags.dutyCycle], m_waveformIndex);
	}
	else { 
		// Use envelope
		return m_envelope.GetDecayLevel() * TestBit8(APU_PULSE_DUTYCYCLE_TO_WAVEFORM[m_parameters.flags.dutyCycle], m_waveformIndex);
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

bool APUPulse::IsLengthCounterPositive() const {
	return m_lengthCounter > 0;
}

void APUPulse::WriteParameters(uint8_t data)
{
	bool oldConstantVolume = m_parameters.flags.constantVolume;
	m_parameters.value = data;
	m_envelope.SetDividerLoad(m_parameters.flags.volumePeriod);
	m_envelope.SetLoop(m_parameters.flags.lengthCounterHalt);
}

void APUPulse::WriteSweep(uint8_t data)
{
	m_sweep.value = data;
	m_sweepReloadFlag = true;
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
	m_envelope.Start();
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
	state.envelopeState = m_envelope.GetState();
	state.sweepDivider = m_sweepDivider;
	state.sweepReloadFlag = m_sweepReloadFlag;

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
	m_envelope.LoadState(state.envelopeState);
	m_sweepDivider = state.sweepDivider;
	m_sweepReloadFlag = state.sweepReloadFlag;

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

uint16_t APUPulse::GetSweepTargetPeriod()
{
	uint16_t currentPeriod = this->GetTimerReload();
	uint16_t changeAmount = ShiftRight16(currentPeriod, m_sweep.flags.shift);
	if (m_sweep.flags.negate) {
		if (m_sweepBehaviour == PULSE_SWEEP_BEHAVIOUR_1) {
			return Add16Bit(currentPeriod, ~changeAmount);
		}
		else {
			return Add16Bit(Add16Bit(currentPeriod, ~changeAmount), 1);
		}
	}
	else {
		return Add16Bit(currentPeriod, changeAmount);
	}
}

bool APUPulse::IsSweepMuting()
{
	return this->GetTimerReload() < APU_PULSE_MIN_TIMER_VALUE || this->GetSweepTargetPeriod() > APU_PULSE_MAX_TIMER_VALUE;
}
