#pragma once

#include <cstdint>
#include "APUEnvelope.h"

constexpr unsigned int APU_NOISE_LONG_MODE_BIT = 1;
constexpr unsigned int APU_NOISE_SHORT_MODE_BIT = 6;
constexpr unsigned int APU_NOISE_LSFR_SET_BIT = 14;
constexpr uint16_t APU_NOISE_LSFR_INITIAL_VALUE = 0x80;
constexpr uint16_t APU_NOISE_TIMER_LOAD_VALUES[16] = { 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};

struct NoiseParametersRegisterFlag {
	uint8_t volumePeriod : 4;
	uint8_t constantVolume : 1;
	uint8_t lengthCounterHalt : 1;
	uint8_t unused : 2;
};

union NoiseParametersRegister {
	NoiseParametersRegisterFlag flags;
	uint8_t value;
};

struct NoisePeriodModeRegisterFlags {
	uint8_t period : 4;
	uint8_t unused : 3;
	uint8_t mode : 1;
};

union NoisePeriodModeRegister {
	NoisePeriodModeRegisterFlags flags;
	uint8_t value;
};

struct NoiseLengthCounterLoadRegisterFlags {
	uint8_t unused : 3;
	uint8_t lengthCounterLoad : 5;
};

union NoiseLengthCounterLoadRegister {
	NoiseLengthCounterLoadRegisterFlags flags;
	uint8_t value;
};


struct APUNoiseState {
	APUNoiseState();

	NoiseParametersRegister parameters;
	NoisePeriodModeRegister periodMode;
	NoiseLengthCounterLoadRegister lengthCounterLoad;

	uint16_t timer;
	uint16_t linearFeedbackShiftRegister;
	uint8_t lengthCounter;
	APUEnvelopeState envelopeState;

	bool silenced;
	bool powerup;
};

class APUNoise {
public:
	APUNoise();

	void SoftReset();
	void HardReset();

	void ClockTimer();
	void ClockEnvelope();
	void ClockLengthCounter();

	uint8_t GetAudioSample();
	void SilenceChannel();
	void PlayChannel();
	bool IsLengthCounterPositive() const;

	void WriteParameters(uint8_t data);
	void WritePeriodMode(uint8_t data);
	void WriteLengthCounterLoad(uint8_t data);

	APUNoiseState GetState() const;
	void LoadState(APUNoiseState& state);

private:
	void ClockLinearFeedbackShiftRegister();

	NoiseParametersRegister m_parameters;
	NoisePeriodModeRegister m_periodMode;
	NoiseLengthCounterLoadRegister m_lengthCounterLoad;

	uint16_t m_timer;
	uint16_t m_linearFeedbackShiftRegister;
	uint8_t m_lengthCounter;
	APUEnvelope m_envelope;

	bool m_silenced;
	bool m_powerup;
};