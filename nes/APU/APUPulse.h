#pragma once

#include <cstdint>
#include <array>

constexpr size_t APU_PULSE_NUM_DUTYCYCLES = 4;
constexpr size_t APU_PULSE_WAVEFORM_LENGTH = 8;
constexpr uint8_t APU_PULSE_MIN_TIMER_VALUE = 8;
constexpr std::array<uint8_t, APU_PULSE_NUM_DUTYCYCLES> APU_PULSE_DUTYCYCLE_TO_WAVEFORM = {
	0b00000001,
	0b00000011,
	0b00001111,
	0b11111100
};


struct PulseParametersRegisterFlags {
	uint8_t volumePeriod : 4;
	uint8_t constantVolume : 1;
	uint8_t lengthCounterHalt : 1;
	uint8_t dutyCycle : 2;
};

union PulseParametersRegister {
	PulseParametersRegisterFlags flags;
	uint8_t value;
};

struct PulseSweepRegisterFlags {
	uint8_t shift : 3;
	uint8_t negate : 1;
	uint8_t period : 3;
	uint8_t enabled : 1;
};

enum PulseSweepBehaviour {
	PULSE_SWEEP_BEHAVIOUR_1,
	PULSE_SWEEP_BEHAVIOUR_2
};

union PulseSweepRegister {
	PulseSweepRegisterFlags flags;
	uint8_t value;
};

struct PulseTimerHighLengthCounterRegisterFlags {
	uint8_t timerHigh : 3;
	uint8_t lengthCounterLoad : 5;
};

union PulseTimerHighLengthCounterRegister {
	PulseTimerHighLengthCounterRegisterFlags flags;
	uint8_t value;
};


struct APUPulseState {
	APUPulseState();

	PulseParametersRegister parameters;
	PulseSweepRegister sweep;
	uint8_t timerLow;
	PulseTimerHighLengthCounterRegister timerHighLengthCounter;

	uint16_t timer;
	uint8_t lengthCounter;
	size_t waveformIndex;

	bool silenced;
};

class APUPulse {
public:
	APUPulse(PulseSweepBehaviour sweepBehaviour);

	void SoftReset();
	void HardReset();

	void ClockTimer();
	void ClockEnvelope();
	void ClockSweep();
	void ClockLengthCounter();
	
	uint8_t GetAudioSample();
	void SilenceChannel();
	void PlayChannel();

	void WriteParameters(uint8_t data);
	void WriteSweep(uint8_t data);
	void WriteTimerLow(uint8_t data);
	void WriteTimerHighLengthCounter(uint8_t data);

	APUPulseState GetState() const;
	void LoadState(APUPulseState& state);

private:
	void ClockWaveformIndex();
	uint16_t GetTimerReload();

	PulseSweepBehaviour m_sweepBehaviour;
	
	PulseParametersRegister m_parameters;
	PulseSweepRegister m_sweep;
	uint8_t m_timerLow;
	PulseTimerHighLengthCounterRegister m_timerHighLengthCounter;

	uint16_t m_timer;
	uint8_t m_lengthCounter;
	size_t m_waveformIndex;

	bool m_silenced;
};