#include "APUEnvelope.h"

APUEnvelopeState::APUEnvelopeState() {
	startFlag = false;
	divider = 0;
	dividerLoad = 0;
	decayLevel = APU_ENVELOPE_DECAY_LOAD;
	loop = false;
}

APUEnvelope::APUEnvelope()
{
	m_startFlag = false;
	m_divider = 0;
	m_dividerLoad = 0;
	m_decayLevel = APU_ENVELOPE_DECAY_LOAD;
	m_loop = false;
}

void APUEnvelope::SoftReset()
{
	m_startFlag = false;
	m_divider = 0;
	m_dividerLoad = 0;
	m_decayLevel = APU_ENVELOPE_DECAY_LOAD;
	m_loop = false;
}

void APUEnvelope::HardReset()
{
	m_startFlag = false;
	m_divider = 0;
	m_dividerLoad = 0;
	m_decayLevel = APU_ENVELOPE_DECAY_LOAD;
	m_loop = false;
}

void APUEnvelope::Start()
{
	m_startFlag = true;
}

void APUEnvelope::Clock()
{
	if (m_startFlag) {
		m_startFlag = false;
		m_decayLevel = APU_ENVELOPE_DECAY_LOAD;
		m_divider = m_dividerLoad;
		return;
	}

	if (m_divider == 0) {
		m_divider = m_dividerLoad;
		if (m_decayLevel == 0) {
			if (m_loop) {
				m_decayLevel = APU_ENVELOPE_DECAY_LOAD;
			}
			return;
		}
		m_decayLevel--;
		return;
	}

	m_divider--;
}

void APUEnvelope::SetDividerLoad(uint8_t load)
{
	m_dividerLoad = load;
}

void APUEnvelope::SetLoop(bool loop)
{
	m_loop = loop;
}

uint8_t APUEnvelope::GetDecayLevel() const {
	return m_decayLevel;
}

APUEnvelopeState APUEnvelope::GetState() const
{
	APUEnvelopeState state;
	state.startFlag = m_startFlag;
	state.divider = m_divider;
	state.dividerLoad = m_dividerLoad;
	state.decayLevel = m_decayLevel;
	state.loop = m_loop;
	return state;
}

void APUEnvelope::LoadState(APUEnvelopeState& state)
{
	m_startFlag = state.startFlag;
	m_divider = state.divider;
	m_dividerLoad = state.dividerLoad;
	m_decayLevel = state.decayLevel;
	m_loop = state.loop;
}
