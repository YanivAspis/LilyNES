#include "APUNoise.h"
#include "LengthCounter.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

APUNoiseState::APUNoiseState() {
	parameters.value = 0;
	periodMode.value = 0;
	lengthCounterLoad.value = 0;

	timer = 0;
	linearFeedbackShiftRegister = 0;
	lengthCounter = 0;

	silenced = true;
	powerup = true;
}

APUNoise::APUNoise()
{
	m_parameters.value = 0;
	m_periodMode.value = 0;
	m_lengthCounterLoad.value = 0;

	m_timer = 0;
	m_linearFeedbackShiftRegister = 0;
	m_lengthCounter = 0;

	m_silenced = true;
	m_powerup = true;
}

void APUNoise::SoftReset()
{
	// Not clear if these should be reset
	m_parameters.value = 0;
	m_periodMode.value = 0;
	m_lengthCounterLoad.value = 0;

	m_timer = 0;
	m_lengthCounter = 0;

	m_silenced = true;
	m_powerup = false;
}

void APUNoise::HardReset()
{
	m_parameters.value = 0;
	m_periodMode.value = 0;
	m_lengthCounterLoad.value = 0;

	m_timer = 0;
	m_linearFeedbackShiftRegister = 0;
	m_lengthCounter = 0;

	m_silenced = true;
	m_powerup = true;
}

void APUNoise::ClockTimer()
{
	if (m_timer == 0) {
		this->ClockLinearFeedbackShiftRegister();
		m_timer = APU_NOISE_TIMER_LOAD_VALUES[m_periodMode.flags.period];
	}
	else {
		m_timer--;
	}
}

void APUNoise::ClockEnvelope()
{
	m_envelope.Clock();
}

void APUNoise::ClockLengthCounter()
{
	if (!m_parameters.flags.lengthCounterHalt && m_lengthCounter > 0) {
		m_lengthCounter--;
	}
}

uint8_t APUNoise::GetAudioSample()
{
	if (m_lengthCounter == 0 || TestBit16(m_linearFeedbackShiftRegister, 0)) {
		return 0;
	}
	else {
		if (m_parameters.flags.constantVolume) {
			return m_parameters.flags.volumePeriod;
		}
		else {
			return m_envelope.GetDecayLevel();
		}
	}
}

void APUNoise::SilenceChannel()
{
	m_silenced = true;
	m_lengthCounter = 0;
}

void APUNoise::PlayChannel()
{
	m_silenced = false;
}

bool APUNoise::IsLengthCounterPositive() const
{
	return m_lengthCounter > 0;
}

void APUNoise::WriteParameters(uint8_t data)
{
	m_parameters.value = data;
	m_parameters.flags.unused = 0;
	m_envelope.SetDividerLoad(m_parameters.flags.volumePeriod);
	m_envelope.SetLoop(m_parameters.flags.lengthCounterHalt);
}

void APUNoise::WritePeriodMode(uint8_t data)
{
	m_periodMode.value = data;
	m_periodMode.flags.unused = 0;
}

void APUNoise::WriteLengthCounterLoad(uint8_t data)
{
	m_lengthCounterLoad.value = data;
	m_lengthCounterLoad.flags.unused = 0;
	if (!m_silenced) {
		m_lengthCounter = APU_LENGTH_COUNTER_VALUES[m_lengthCounterLoad.flags.lengthCounterLoad];
	}
	m_envelope.Start();
}

APUNoiseState APUNoise::GetState() const
{
	APUNoiseState state;

	state.parameters.value = m_parameters.value;
	state.periodMode.value = m_periodMode.value;
	state.lengthCounterLoad.value = m_lengthCounterLoad.value;

	state.timer = m_timer;
	state.linearFeedbackShiftRegister = m_linearFeedbackShiftRegister;
	state.lengthCounter = m_lengthCounter;
	state.envelopeState = m_envelope.GetState();

	state.silenced = m_silenced;
	state.powerup = m_powerup;

	return state;
}

void APUNoise::LoadState(APUNoiseState& state)
{
	m_parameters.value = state.parameters.value;
	m_periodMode.value = state.periodMode.value;
	m_lengthCounterLoad.value = state.lengthCounterLoad.value;

	m_timer = state.timer;
	m_linearFeedbackShiftRegister = state.linearFeedbackShiftRegister;
	m_lengthCounter = state.lengthCounter;
	m_envelope.LoadState(state.envelopeState);

	m_silenced = state.silenced;
	m_powerup = state.powerup;
}

void APUNoise::ClockLinearFeedbackShiftRegister()
{
	if (m_powerup) {
		m_powerup = false;
		m_linearFeedbackShiftRegister = APU_NOISE_LSFR_INITIAL_VALUE;
		return;
	}

	bool xorResult = false;
	if (m_periodMode.flags.mode) {
		xorResult = TestBit16(m_linearFeedbackShiftRegister, 0) ^ TestBit16(m_linearFeedbackShiftRegister, APU_NOISE_SHORT_MODE_BIT);
	}
	else {
		xorResult = TestBit16(m_linearFeedbackShiftRegister, 0) ^ TestBit16(m_linearFeedbackShiftRegister, APU_NOISE_LONG_MODE_BIT);
	}
	m_linearFeedbackShiftRegister = ShiftRight16(m_linearFeedbackShiftRegister, 1);
	if (xorResult) {
		SetBit16(m_linearFeedbackShiftRegister, APU_NOISE_LSFR_SET_BIT);
	}
}
