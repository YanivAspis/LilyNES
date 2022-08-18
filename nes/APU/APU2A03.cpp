#include "APU2A03.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

APUState::APUState() {
	frameCounter = 0;

	controlRegister.value = 0;
	statusRegister.value = 0;
	frameCounterRegister.value = 0;
}

APU2A03::APU2A03(CPU2A03* cpu) : BusDevice(std::list<AddressRange>({AddressRange(APU_BEGIN_ADDRESS_1, APU_END_ADDRESS_1), AddressRange(APU_BEGIN_ADDRESS_2, APU_END_ADDRESS_2) })),
		m_pulse1(PULSE_SWEEP_BEHAVIOUR_1)
{
	m_cpu = cpu;
	m_frameCounter = 0;
	m_controlRegister.value = 0;
	m_statusRegister.value = 0;
	m_frameCounterRegister.value = 0;
}

APU2A03::~APU2A03() {
	m_cpu = nullptr;
}

void APU2A03::SoftReset()
{
	m_pulse1.SoftReset();

	m_frameCounter = 0; // Not clear if I should reset the frame counter or not

	this->ControlRegisterWrite(0); // Silence all channels
	m_statusRegister.value = 0;
	m_frameCounterRegister.flags.irqInhibit = 0; // I think I'm only supposed to allow IRQ's, but not change the mode
}

void APU2A03::HardReset()
{
	m_pulse1.HardReset();

	m_frameCounter = 0; // Not clear if I should set the frame counter to 15

	this->ControlRegisterWrite(0); // Silence all channels
	m_statusRegister.value = 0;
	m_frameCounterRegister.value = 0;
}

float APU2A03::GetAudioSample()
{
	uint8_t pulse1Sample = m_pulse1.GetAudioSample();
	uint8_t pulse2Sample = 0;
	uint8_t triangleSample = 0;
	uint8_t noiseSample = 0;
	uint8_t DMCSample = 0;
	return this->MixSamples(pulse1Sample, pulse2Sample, triangleSample, noiseSample, DMCSample);
	

	/*
	static double globalTime = 0;
	float sample = 1.0 * sin(2 * 3.14159265359 * 440.0 * globalTime);
	globalTime += 1.0 / 44100;
	return sample;
	*/
}

APUState APU2A03::GetState() const
{
	APUState state;

	state.pulse1State = m_pulse1.GetState();

	state.frameCounter = m_frameCounter;

	state.controlRegister.value = m_controlRegister.value;
	state.statusRegister.value = m_statusRegister.value;
	state.frameCounterRegister.value = m_frameCounterRegister.value;

	return state;
}

void APU2A03::LoadState(APUState& state)
{
	m_pulse1.LoadState(state.pulse1State);

	m_frameCounter = state.frameCounter;

	m_controlRegister.value = state.controlRegister.value;
	m_statusRegister.value = state.statusRegister.value;
	m_frameCounterRegister.value = ClearLowerBits8(state.frameCounterRegister.value, 6);
}


float APU2A03::MixSamples(uint8_t pulse1Sample, uint8_t pulse2Sample, uint8_t triangleSample, uint8_t noiseSample, uint8_t DMCSample) {
	float pulseFactor = pulse1Sample + pulse2Sample;
	float pulse_out = 95.88 * pulseFactor / (8128 + 100 * pulseFactor);
	float tndFactor = (float)triangleSample / 8227 + (float)noiseSample / 12241 + (float)DMCSample / 22638;
	float tnd_out = 159.79 * tndFactor / (1 + 100 * tndFactor);
	return pulse_out + tnd_out;
}