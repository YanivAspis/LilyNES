#pragma once

#include <cstdint>

constexpr uint8_t APU_TRIANGLE_MIN_TIMER_VALUE = 2;
constexpr uint16_t APU_TRIANGLE_MAX_TIMER_VALUE = 0x7FF;

constexpr size_t APU_TRIANGLE_WAVEFORM_LENGTH = 32;
constexpr uint8_t APU_TRIANGLE_WAVEFORM[APU_TRIANGLE_WAVEFORM_LENGTH] = { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};


struct TriangleCounterParametersRegisterFlags {
	uint8_t linearCounterLoad : 7;
	uint8_t controlFlag : 1; // Also length counter halt
};

union TriangleCounterParametersRegister {
	TriangleCounterParametersRegisterFlags flags;
	uint8_t value;
};

struct TriangleTimerHighLengthCounterRegisterFlags {
	uint8_t timerHigh : 3;
	uint8_t lengthCounterLoad : 5;
};

union TriangleTimerHighLengthCounterRegister {
	TriangleTimerHighLengthCounterRegisterFlags flags;
	uint8_t value;
};


struct APUTriangleState {
	APUTriangleState();

	TriangleCounterParametersRegister counterParameters;
	uint8_t timerLow;
	TriangleTimerHighLengthCounterRegister timerHighLengthCounter;

	uint16_t timer;
	uint8_t linearCounter;
	bool linearCounterReloadFlag;
	uint8_t lengthCounter;
	size_t waveformIndex;

	bool silenced;
};


class APUTriangle {
public:
	APUTriangle();

	void SoftReset();
	void HardReset();

	void ClockTimer();
	void ClockLinearCounter();
	void ClockLengthCounter();

	uint8_t GetAudioSample();
	void SilenceChannel();
	void PlayChannel();
	bool IsLengthCounterPositive() const;

	void WriteCounterParameters(uint8_t data);
	void WriteTimerLow(uint8_t data);
	void WriteTimerHighLengthCounter(uint8_t data);

	APUTriangleState GetState() const;
	void LoadState(APUTriangleState& state);

private:
	void ClockWaveformIndex();
	uint16_t GetTimerReload();

	TriangleCounterParametersRegister m_counterParameters;
	uint8_t m_timerLow;
	TriangleTimerHighLengthCounterRegister m_timerHighLengthCounter;

	uint16_t m_timer;
	uint8_t m_linearCounter;
	bool m_linearCounterReloadFlag;
	uint8_t m_lengthCounter;
	size_t m_waveformIndex;

	bool m_silenced;
};
