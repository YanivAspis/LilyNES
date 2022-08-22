#include <assert.h>

#include "APUDMC.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

APUDMCState::APUDMCState() {
	paraemters.value = 0;
	sampleAddress = 0;
	sampleLength = 0;

	timer = 0;
	irqFlag = false;

	sampleBuffer = 0;
	sampleBufferEmpty = true;
	bytesRemaining = 0;
	currentSampleAddress = 0;
	sampleNeeded = false;

	outputLevel = 0;
	outputShiftRegister = 0;
	shifterBitsRemaining = APU_DMC_OUTPUT_SHIFTER_SIZE;
	outputSilenceFlag = true;

	silenced = true;
}

APUDMC::APUDMC(CPU2A03* cpu)
{
	m_cpu = cpu;
	m_bus = nullptr;

	m_paraemters.value = 0;
	m_sampleAddress = 0;
	m_sampleLength = 0;

	m_timer = 0;
	m_irqFlag = false;

	m_sampleBuffer = 0;
	m_sampleBufferEmpty = true;
	m_bytesRemaining = 0;
	m_currentSampleAddress = 0;
	m_sampleNeeded = false;

	m_outputLevel = 0;
	m_outputShiftRegister = 0;
	m_shifterBitsRemaining = 0;
	m_outputSilenceFlag = true;

	m_silenced = true;
}

APUDMC::~APUDMC()
{
	m_cpu = nullptr;
	m_bus = nullptr;
}

void APUDMC::ConnectToBus(Bus* bus) {
	assert(m_bus == nullptr);
	m_bus = bus;
}

void APUDMC::DisconnectFromBus() {
	m_bus = nullptr;
}

void APUDMC::SoftReset()
{
	m_irqFlag = false;
	m_bytesRemaining = 0;
	m_outputLevel = ClearUpperBits8(m_outputLevel, 1);
	m_silenced = true;
}

void APUDMC::HardReset()
{
	m_paraemters.value = 0;
	m_sampleAddress = 0;
	m_sampleLength = 0;

	m_timer = 0;
	m_irqFlag = false;

	m_sampleBuffer = 0;
	m_sampleBufferEmpty = true;
	m_bytesRemaining = 0;
	m_currentSampleAddress = 0;
	m_sampleNeeded = false;

	m_outputLevel = 0;
	m_outputShiftRegister = 0;
	m_shifterBitsRemaining = 8;
	m_outputSilenceFlag = true;

	m_silenced = true;
}

void APUDMC::ClockTimer()
{
	if (m_timer == 0) {
		this->ClockOutputUnit();
		m_timer = APU_DMC_TIMER_RELOAD_VALUES[m_paraemters.flags.rate];
	}
	else {
		m_timer--;
	}
}

uint8_t APUDMC::GetAudioSample()
{
	return m_outputLevel;
}

void APUDMC::SilenceChannel()
{
	m_bytesRemaining = 0;
	m_silenced = true;
	m_cpu->AcknowledgeIRQ(APU_DMC_IRQ_ID);
	m_irqFlag = false;
}

void APUDMC::PlayChannel()
{
	m_silenced = false;
	if (m_bytesRemaining == 0) {
		this->StartPlay();
	}
}

void APUDMC::WriteParameters(uint8_t data)
{
	m_paraemters.value = data;
	m_paraemters.flags.unused = 0;
	if (!m_paraemters.flags.irqEnabled) {
		m_cpu->AcknowledgeIRQ(APU_DMC_IRQ_ID);
		m_irqFlag = false;
	}
}

void APUDMC::WriteDirectLoad(uint8_t data)
{
	m_outputLevel = data;
	ClearBit8(m_outputLevel, 7);
}

void APUDMC::WriteSampleAddress(uint8_t data)
{
	m_sampleAddress = ShiftLeft16(data, APU_DMC_SAMPLE_ADDRESS_SHIFT_AMOUNT);
	m_sampleAddress = Add16Bit(APU_DMC_SAMPLE_BASE_ADDRESS, m_sampleAddress);
}

void APUDMC::WriteSampleLength(uint8_t data)
{
	m_sampleLength = ShiftLeft16(data, APU_DMC_SAMPLE_LENGTH_SHIFT_AMOUNT);
}

bool APUDMC::IsDMCActive() const
{
	return m_bytesRemaining > 0;
}

bool APUDMC::IsInterruptAsserted() const
{
	return m_irqFlag;
}

void APUDMC::ClearInterruptFlag()
{
	m_cpu->AcknowledgeIRQ(APU_DMC_IRQ_ID);
	m_irqFlag = false;
}

bool APUDMC::IsRequestingSample()
{
	bool returnValue = m_sampleNeeded;
	m_sampleNeeded = false;
	return returnValue;
}

APUDMCState APUDMC::GetState() const
{
	APUDMCState state;

	state.paraemters.value = m_paraemters.value;
	state.sampleAddress = m_sampleAddress;
	state.sampleLength = m_sampleLength;

	state.timer = m_timer;
	state.irqFlag = m_irqFlag;

	state.sampleBuffer = m_sampleBuffer;
	state.sampleBufferEmpty = m_sampleBufferEmpty;
	state.bytesRemaining = m_bytesRemaining;
	state.currentSampleAddress = m_currentSampleAddress;
	state.sampleNeeded = m_sampleNeeded;

	state.outputLevel = m_outputLevel;
	state.outputShiftRegister = m_outputShiftRegister;
	state.shifterBitsRemaining = m_shifterBitsRemaining;
	state.outputSilenceFlag = m_outputSilenceFlag;

	state.silenced = m_silenced;

	return state;
}

void APUDMC::LoadState(APUDMCState& state)
{
	m_paraemters.value = state.paraemters.value;
	m_sampleAddress = state.sampleAddress;
	m_sampleLength = state.sampleLength;

	m_timer = state.timer;
	m_irqFlag = state.irqFlag;

	m_sampleBuffer = state.sampleBuffer;
	m_sampleBufferEmpty = state.sampleBufferEmpty;
	m_bytesRemaining = state.bytesRemaining;
	m_currentSampleAddress = state.currentSampleAddress;
	m_sampleNeeded = state.sampleNeeded;

	m_outputLevel = state.outputLevel;
	m_outputShiftRegister = state.outputShiftRegister;
	m_shifterBitsRemaining = state.shifterBitsRemaining;
	m_outputSilenceFlag = state.outputSilenceFlag;

	m_silenced = state.silenced;
}

void APUDMC::ClockOutputUnit()
{
	if (!m_outputSilenceFlag) {
		this->ChangeOutputLevel();
	}
	m_outputShiftRegister = ShiftRight8(m_outputShiftRegister, 1);
	m_shifterBitsRemaining--;
	if (m_shifterBitsRemaining == 0) {
		this->OutputUnitNewCycle();
	}
}

void APUDMC::OutputUnitNewCycle()
{
	m_shifterBitsRemaining = APU_DMC_OUTPUT_SHIFTER_SIZE;
	if (m_sampleBufferEmpty) {
		m_outputSilenceFlag = true;
	}
	else {
		m_outputSilenceFlag = false;
		this->FillShiftRegisterFromSampleBuffer();
	}
}

void APUDMC::ChangeOutputLevel()
{
	if (TestBit8(m_outputShiftRegister, 0)) {
		// Increase output
		if (m_outputLevel + APU_DMC_OUTPUT_DELTA <= APU_DMC_MAX_OUTPUT_LEVEL) {
			m_outputLevel += APU_DMC_OUTPUT_DELTA;
		}
	}
	else {
		// Decrease output
		if (m_outputLevel >= APU_DMC_OUTPUT_DELTA) {
			m_outputLevel -= APU_DMC_OUTPUT_DELTA;
		}
	}
}

void APUDMC::FillShiftRegisterFromSampleBuffer()
{
	m_outputShiftRegister = m_sampleBuffer;
	m_sampleBufferEmpty = true;
	if (m_bytesRemaining > 0) {
		this->GetNewSampleIntoBuffer();
	}

}

void APUDMC::GetNewSampleIntoBuffer() {
	m_sampleNeeded = true;
	m_sampleBuffer = m_bus->Read(m_currentSampleAddress, true);
	Inc16Bit(m_currentSampleAddress);
	if (m_currentSampleAddress == 0) {
		m_currentSampleAddress = APU_DMC_SAMPLE_OVERFLOW_ADDRESS;
	}
	m_sampleBufferEmpty = false;
	m_bytesRemaining--;
	if (m_bytesRemaining == 0) {
		this->HandleZeroBytesRemaining();
	}
}

void APUDMC::HandleZeroBytesRemaining()
{
	if (m_paraemters.flags.loop) {
		this->StartPlay();
	}
	else if (m_paraemters.flags.irqEnabled) {
		m_cpu->RaiseIRQ(APU_DMC_IRQ_ID);
		m_irqFlag = true;
	}
}

void APUDMC::StartPlay()
{
	if (!m_silenced) {
		m_currentSampleAddress = m_sampleAddress;
		m_bytesRemaining = m_sampleLength;
		if (m_sampleBufferEmpty) {
			this->GetNewSampleIntoBuffer();
		}
	}
}
