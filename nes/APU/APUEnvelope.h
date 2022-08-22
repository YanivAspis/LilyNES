#pragma once

#include <cstdint>

constexpr uint8_t APU_ENVELOPE_DECAY_LOAD = 0xF;

struct APUEnvelopeState {
	APUEnvelopeState();
	bool startFlag;
	uint8_t divider;
	uint8_t dividerLoad;
	uint8_t decayLevel;
	bool loop;
};

class APUEnvelope {
public:
	APUEnvelope();

	void SoftReset();
	void HardReset();

	void Start();
	void Clock();
	void SetDividerLoad(uint8_t load);
	void SetLoop(bool loop);
	uint8_t GetDecayLevel() const;

	APUEnvelopeState GetState() const;
	void LoadState(APUEnvelopeState& state);

private:
	bool m_startFlag;
	uint8_t m_divider;
	uint8_t m_dividerLoad;
	uint8_t m_decayLevel;
	bool m_loop;
};
