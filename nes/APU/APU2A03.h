#pragma once

#include "../BusDevice.h"
#include "../CPU/CPU2A03.h"
#include "APUPulse.h"
#include "APUTriangle.h"
#include "APUNoise.h"
#include "APUDMC.h"

constexpr uint16_t APU_BEGIN_ADDRESS_1 = 0x4000;
constexpr uint16_t APU_END_ADDRESS_1 = 0x4013;
constexpr uint16_t APU_BEGIN_ADDRESS_2 = 0x4015;
constexpr uint16_t APU_END_ADDRESS_2 = 0x4015;

constexpr uint16_t APU_PULSE_1_PARAMETERS_ADDRESS = 0x4000;
constexpr uint16_t APU_PULSE_1_SWEEP_ADDRESS = 0x4001;
constexpr uint16_t APU_PULSE_1_TIMER_LOW_ADDRESS = 0x4002;
constexpr uint16_t APU_PULSE_1_TIMER_HIGH_LENGTH_COUNTER_ADDRESS = 0x4003;

constexpr uint16_t APU_PULSE_2_PARAMETERS_ADDRESS = 0x4004;
constexpr uint16_t APU_PULSE_2_SWEEP_ADDRESS = 0x4005;
constexpr uint16_t APU_PULSE_2_TIMER_LOW_ADDRESS = 0x4006;
constexpr uint16_t APU_PULSE_2_TIMER_HIGH_LENGTH_COUNTER_ADDRESS = 0x4007;

constexpr uint16_t APU_TRIANGLE_COUNTER_PARAMETERS_ADDRESS = 0x4008;
constexpr uint16_t APU_UNUSED_REGISTER_1_ADDRESS = 0x4009;
constexpr uint16_t APU_TRIANGLE_TIMER_LOW_ADDRESS = 0x400A;
constexpr uint16_t APU_TRIANGLE_TIMER_HIGH_LENGTH_COUNTER_ADDRESS = 0x400B;

constexpr uint16_t APU_NOISE_PARAMETERS_ADDRESS = 0x400C;
constexpr uint16_t APU_UNUSED_REGISTER_2_ADDRESS = 0x400D;
constexpr uint16_t APU_NOISE_PERIOD_MODE_ADDRESS = 0x400E;
constexpr uint16_t APU_NOISE_LENGTH_COUNTER_LOAD_ADDRESS = 0x400F;

constexpr uint16_t APU_DMC_PARAMETERS_ADDRESS = 0x4010;
constexpr uint16_t APU_DMC_DIRECT_LOAD_ADDRESS = 0x4011;
constexpr uint16_t APU_DMC_SAMPLE_ADDR_ADDRESS = 0x4012;
constexpr uint16_t APU_DMC_SAMPLE_LENGTH_ADDRESS = 0x4013;

constexpr uint16_t APU_CONTROL_STATUS_ADDRESS = 0x4015;
constexpr uint16_t APU_FRAME_COUNTER_ADDRESS = 0x4017;


constexpr unsigned int APU_FRAME_COUNTER_QUARTER = 7457; // 2 * 3728.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_HALF = 14913; // 2 * 7456.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_THREE_QUARTERS = 22371; // 2 * 11185.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_4_STEP_PENULTIMATE_FRAME = 29828; // 2 * 14914 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_4_STEP_LAST_FRAME = 29829; // 2 * 14914.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_5_STEP_LAST_FRAME = 37281; // 2 * 18640.5 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_4_STEP_MAX = 29830; // 2 * 14915 APU Cycles
constexpr unsigned int APU_FRAME_COUNTER_5_STEP_MAX = 37282; // 2 * 18641 APU Cycles

constexpr unsigned int APU_FRAME_COUNTER_WRITE_CYCLES_EVEN = 3;
constexpr unsigned int APU_FRAME_COUNTER_WRITE_CYCLES_ODD = 4;

constexpr char APU_FRAME_IRQ_ID[10] = "APU_FRAME";



struct APUControlRegisterFlags {
	uint8_t enablePulse1 : 1;
	uint8_t enablePulse2 : 1;
	uint8_t enableTriangle : 1;
	uint8_t enableNoise : 1;
	uint8_t enableDMC : 1;
	uint8_t unused : 3;
};

union APUControlRegister {
	APUControlRegisterFlags flags;
	uint8_t value;
};

struct APUStatusRegisterFlags {
	uint8_t pulse1LengthPositive : 1;
	uint8_t pulse2LengthPositive : 1;
	uint8_t triangleLengthPositive : 1;
	uint8_t noiseLengthPositive : 1;
	uint8_t DMCActive : 1;
	uint8_t unused : 1;
	uint8_t frameInterrupt : 1;
	uint8_t DMCInterrupt : 1;
};

union APUStatusRegister {
	APUStatusRegisterFlags flags;
	uint8_t value;
};

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
	APUState();

	APUPulseState pulse1State;
	APUPulseState pulse2State;
	APUTriangleState triangleState;
	APUNoiseState noiseState;
	APUDMCState dmcState;

	unsigned int frameCounter;
	unsigned int frameCounterWriteCycles;

	APUControlRegister controlRegister;
	APUStatusRegister statusRegister;
	FrameCounterRegister frameCounterRegister;
};

class APU2A03 : public BusDevice {
public:
	APU2A03(CPU2A03* cpu);
	~APU2A03();

	void DMCConnectToBus(Bus* bus);
	void DMCDisconnectFromBus();

	void SoftReset() override;
	void HardReset() override;

	void Read(uint16_t address, uint8_t& data) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	void Clock();
	float GetAudioSample();

	bool IsDMCRequestingSample();

	APUState GetState() const;
	void LoadState(APUState& state);

private:
	void DoFrameCounterQuarter();
	void DoFrameCounterHalf();
	void DoFrameCounterThreeQuarters();
	void DoFrameCounterMode4Last();
	void DoFrameCounterMode5Last();
	void IncrementFrameCounter();
	void GenerateFrameInterrupt();
	
	void ControlRegisterWrite(uint8_t data);
	uint8_t StatusRegisterRead();
	void FrameCounterRegisterWrite(uint8_t data);

	float MixSamples(uint8_t pulse1Sample, uint8_t pulse2Sample, uint8_t triangleSample, uint8_t noiseSample, uint8_t DMCSample);

	CPU2A03* m_cpu;

	APUPulse m_pulse1;
	APUPulse m_pulse2;
	APUTriangle m_triangle;
	APUNoise m_noise;
	APUDMC m_dmc;

	unsigned int m_frameCounter;
	unsigned int m_frameCounterWriteCycles;

	APUControlRegister m_controlRegister;
	APUStatusRegister m_statusRegister;
	FrameCounterRegister m_frameCounterRegister;
};