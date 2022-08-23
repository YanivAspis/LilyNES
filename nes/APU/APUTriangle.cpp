#include "APUTriangle.h"
#include "LengthCounter.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

APUTriangleState::APUTriangleState() {
	counterParameters.value = 0;
	timerLow = 0;
	timerHighLengthCounter.value = 0;

	timer = 0;
	linearCounter = 0;
	linearCounterReloadFlag = true;
	lengthCounter = 0;
	waveformIndex = 0;

	silenced = true;
}

APUTriangle::APUTriangle()
{
	m_counterParameters.value = 0;
	m_timerLow = 0;
	m_timerHighLengthCounter.value = 0;

	m_timer = 0;
	m_linearCounter = 0;
	m_linearCounterReloadFlag = true;
	m_lengthCounter = 0;
	m_waveformIndex = 0;

	m_silenced = true;
}

void APUTriangle::SoftReset()
{
	m_lengthCounter = 0;
	m_waveformIndex = 0; // Phase reset

	m_silenced = true;
}

void APUTriangle::HardReset()
{
	m_counterParameters.value = 0;
	m_timerLow = 0;
	m_timerHighLengthCounter.value = 0;

	m_timer = 0;
	m_linearCounter = 0;
	m_linearCounterReloadFlag = true;
	m_lengthCounter = 0;
	m_waveformIndex = 0;

	m_silenced = true;
}

void APUTriangle::ClockTimer()
{
	if (m_timer == 0) {
		this->ClockWaveformIndex();
		m_timer = this->GetTimerReload();
	}
	else {
		m_timer--;
	}
}

void APUTriangle::ClockLinearCounter()
{
	if (m_linearCounterReloadFlag) {
		m_linearCounter = m_counterParameters.flags.linearCounterLoad;
	}
	else if (m_linearCounter > 0) {
		m_linearCounter--;
	}

	if (!m_counterParameters.flags.controlFlag) {
		m_linearCounterReloadFlag = false;
	}
}

void APUTriangle::ClockLengthCounter()
{
	if (!m_counterParameters.flags.controlFlag && m_lengthCounter > 0) {
		m_lengthCounter--;
	}
}

uint8_t APUTriangle::GetAudioSample()
{
	// Silence the channel on extreme frequencies to protect speakers
	// Possibly redundant due to high-pass/low-pass filtering
	uint16_t timerLoad = this->GetTimerReload();
	if (timerLoad < APU_TRIANGLE_MIN_TIMER_VALUE || timerLoad > APU_TRIANGLE_MAX_TIMER_VALUE) {
		return 0;
	}
	else {
		return APU_TRIANGLE_WAVEFORM[m_waveformIndex];
	}
}

void APUTriangle::SilenceChannel()
{
	m_silenced = true;
	m_lengthCounter = 0;
}

void APUTriangle::PlayChannel()
{
	m_silenced = false;
}

bool APUTriangle::IsLengthCounterPositive() const
{
	return m_lengthCounter > 0;
}

void APUTriangle::WriteCounterParameters(uint8_t data)
{
	m_counterParameters.value = data;
}

void APUTriangle::WriteTimerLow(uint8_t data)
{
	m_timerLow = data;
}

void APUTriangle::WriteTimerHighLengthCounter(uint8_t data)
{
	m_timerHighLengthCounter.value = data;
	m_linearCounterReloadFlag = true;
	if (!m_silenced) {
		m_lengthCounter = APU_LENGTH_COUNTER_VALUES[m_timerHighLengthCounter.flags.lengthCounterLoad];
	}
}

APUTriangleState APUTriangle::GetState() const
{
	APUTriangleState state;

	state.counterParameters.value = m_counterParameters.value;
	state.timerLow = m_timerLow;
	state.timerHighLengthCounter.value = m_timerHighLengthCounter.value;

	state.timer = m_timer;
	state.linearCounter = m_linearCounter;
	state.linearCounterReloadFlag = m_linearCounterReloadFlag;
	state.lengthCounter = m_lengthCounter;
	state.waveformIndex = m_waveformIndex;

	state.silenced = m_silenced;

	return state;
}

void APUTriangle::LoadState(APUTriangleState& state)
{
	m_counterParameters.value = state.counterParameters.value;
	m_timerLow = state.timerLow;
	m_timerHighLengthCounter.value = state.timerHighLengthCounter.value;

	m_timer = state.timer;
	m_linearCounter = state.linearCounter;
	m_linearCounterReloadFlag = state.linearCounterReloadFlag;
	m_lengthCounter = state.lengthCounter;
	m_waveformIndex = state.waveformIndex;

	m_silenced = state.silenced;
}

void APUTriangle::ClockWaveformIndex()
{
	if (m_lengthCounter > 0 && m_linearCounter > 0) {
		if (m_waveformIndex == APU_TRIANGLE_WAVEFORM_LENGTH - 1) {
			m_waveformIndex = 0;
		}
		else {
			m_waveformIndex++;
		}
	}
}

uint16_t APUTriangle::GetTimerReload()
{
	return CombineBytes(m_timerLow, m_timerHighLengthCounter.flags.timerHigh);
}
