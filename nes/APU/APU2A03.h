#pragma once

#include "../BusDevice.h"
#include "../CPU/CPU2A03.h"

constexpr uint16_t APU_BEGIN_ADDRESS_1 = 0x4000;
constexpr uint16_t APU_END_ADDRESS_1 = 0x4013;
constexpr uint16_t APU_BEGIN_ADDRESS_2 = 0x4015;
constexpr uint16_t APU_END_ADDRESS_2 = 0x4015;

constexpr unsigned int APU_FRAME_COUNTER_QUARTER = 7457; // 2 * 3728.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_HALF = 14913; // 2 * 7456.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_THREE_QUARTERS = 22371; // 2 * 11185.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_4_STEP_PENULTIMATE_FRAME = 29828; // 2 * 14914 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_4_STEP_LAST_FRAME = 29829; // 2 * 14914.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_5_STEP_LAST_FRAME = 37281; // 2 * 18640.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_4_STEP_MAX = 29830; // 2 * 14915 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_5_STEP_MAX = 37282; // 2 * 18641 APU Cycles

enum FrameCounterMode {
	FRAME_COUNTER_MODE_4_STEP,
	FRAME_COUNTER_MODE_5_STEP,
};

struct FrameCounterFlags {
	uint8_t unused : 6;
	uint8_t irqInhibit : 1;
	uint8_t mode : 1;
};

union FrameCounterRegister {
	FrameCounterFlags flags;
	uint8_t value;
};

struct APUState {
	unsigned int frameCounter;
	bool frameIRQSent;

	FrameCounterRegister frameCounterRegister;
};

class APU2A03 : public BusDevice {
public:
	APU2A03(CPU2A03* cpu);
	~APU2A03();

	void SoftReset() override;
	void HardReset() override;

	void Read(uint16_t address, uint8_t& data) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	void Clock();
	float GetAudioSample();

	APUState GetState() const;
	void LoadState(APUState& state);

private:
	void APUClock();

	void DoFrameCounterZero();
	void DoFrameCounterOne();
	void DoFrameCounterQuarter();
	void DoFrameCounterHalf();
	void DoFrameCounterThreeQuarters();
	void DoFrameCounterMode4Penultimate();
	void DoFrameCounterMode4Last();
	void DoFrameCounterMode5Last();
	void IncrementFrameCounter();
	void GenerateFrameInterrupt();
	

	void FrameCounterRegisterWrite(uint8_t data);

	float MixSamples(uint8_t pulse1Sample, uint8_t pulse2Sample, uint8_t triangleSample, uint8_t noiseSample, uint8_t DMCSample);

	CPU2A03* m_cpu;

	unsigned int m_frameCounter;
	bool m_frameIRQSent;

	FrameCounterRegister m_frameCounterRegister;
};