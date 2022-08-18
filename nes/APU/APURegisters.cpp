#include "APU2A03.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

void APU2A03::Read(uint16_t address, uint8_t& data)
{
	if (address == APU_CONTROL_STATUS_ADDRESS) {
		// Not sure if unused bit is meant to be open bus
		data = this->StatusRegisterRead();
	}
}

void APU2A03::Write(uint16_t address, uint8_t data)
{
	switch (address) {
	case APU_PULSE_1_PARAMETERS_ADDRESS:
		m_pulse1.WriteParameters(data);
		break;

	case APU_PULSE_1_SWEEP_ADDRESS:
		m_pulse1.WriteSweep(data);
		break;

	case APU_PULSE_1_TIMER_LOW_ADDRESS:
		m_pulse1.WriteTimerLow(data);
		break;

	case APU_PULSE_1_TIMER_HIGH_LENGTH_COUNTER_ADDRESS:
		m_pulse1.WriteTimerHighLengthCounter(data);
		break;

	case APU_PULSE_2_PARAMETERS_ADDRESS:
		break;

	case APU_PULSE_2_SWEEP_ADDRESS:
		break;

	case APU_PULSE_2_TIMER_LOW_ADDRESS:
		break;

	case APU_PULSE_2_TIMER_HIGH_LENGTH_COUNTER_ADDRESS:
		break;

	case APU_TRIANGLE_COUNTER_PARAMETERS_ADDRESS:
		break;

	case APU_TRIANGLE_TIMER_LOW_ADDRESS:
		break;

	case APU_TRIANGLE_TIMER_HIGH_LENGTH_COUNTER_ADDRESS:
		break;

	case APU_NOISE_PARAMETERS_ADDRESS:
		break;

	case APU_NOISE_PERIOD_LOOP_ADDRESS:
		break;

	case APU_NOISE_LENGTH_COUNTER_ADDRESS:
		break;

	case APU_DMC_PARAMETERS_ADDRESS:
		break;

	case APU_DMC_DIRECT_LOAD_ADDRESS:
		break;

	case APU_DMC_SAMPLE_ADDR_ADDRESS:
		break;

	case APU_DMC_SAMPLE_LENGTH_ADDRESS:
		break;

	case APU_CONTROL_STATUS_ADDRESS:
		this->ControlRegisterWrite(data);
		break;

	case APU_FRAME_COUNTER_ADDRESS:
		this->FrameCounterRegisterWrite(data);
		break;
	}
}

uint8_t APU2A03::Probe(uint16_t address)
{
	return 0;
}

void APU2A03::ControlRegisterWrite(uint8_t data) {
	m_controlRegister.value = data;
	m_controlRegister.flags.unused = 0;

	if (m_controlRegister.flags.enablePulse1) {
		m_pulse1.PlayChannel();
	}
	else {
		m_pulse1.SilenceChannel();
	}
	if (m_controlRegister.flags.enablePulse2) {
		// Enable Pulse 2
	}
	else {
		// Silence Pulse 2
	}
	if (m_controlRegister.flags.enableTriangle) {
		// Enable Triangle
	}
	else {
		// Silence Triangle
	}
	if (m_controlRegister.flags.enableNoise) {
		// Enable Noise
	}
	else {
		// Silence Noise
	}
	if (m_controlRegister.flags.enableDMC) {
		// DMC restarted immediately only if bytes remaining is 0 (1-byte buffer will finish playing)
	}
	else {
		// Set DMC bytes remaining to 0
	}

	// For some reasong, writing to this register clears the DMC Interrupt flag
	m_statusRegister.flags.DMCInterrupt = 0;
}

uint8_t APU2A03::StatusRegisterRead() {
	// Check if length counters are positive and DMC is active

	uint8_t returnValue = m_statusRegister.value;

	if (m_statusRegister.flags.frameInterrupt) {
		m_statusRegister.flags.frameInterrupt = 0;
		m_cpu->AcknowledgeIRQ(APU_FRAME_IRQ_ID);
	}

	return returnValue;
}

void APU2A03::FrameCounterRegisterWrite(uint8_t data) {
	m_frameCounterRegister.value = data;
	m_frameCounterRegister.flags.unused = 0;

	// Not sure if pending IRQ's should be acknowledged
	if (m_frameCounterRegister.flags.irqInhibit) {
		m_statusRegister.flags.frameInterrupt = 0;
		m_cpu->AcknowledgeIRQ(APU_FRAME_IRQ_ID);
	}

	// Frame counter is reset and possibly generate quarter and half frame signals
	// TODO: Do this after 3 cycles?
	m_frameCounter = 0;
	if (m_frameCounterRegister.flags.mode == FRAME_COUNTER_MODE_5_STEP) {
		this->DoFrameCounterHalf();
	}
}