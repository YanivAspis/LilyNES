#pragma once

#include <cstdint>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "../CPU/CPU2A03.h"

constexpr uint8_t APU_DMC_OUTPUT_DELTA = 2;
constexpr uint8_t APU_DMC_MAX_OUTPUT_LEVEL = 0x7F;

constexpr size_t APU_DMC_OUTPUT_SHIFTER_SIZE = 8;
constexpr uint16_t APU_DMC_TIMER_RELOAD_VALUES[16] = { 428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54};

constexpr uint16_t APU_DMC_SAMPLE_OVERFLOW_ADDRESS = 0x8000;
constexpr uint16_t APU_DMC_SAMPLE_BASE_ADDRESS = 0xC000;
constexpr uint16_t APU_DMC_SAMPLE_ADDRESS_SHIFT_AMOUNT = 6;
constexpr uint16_t APU_DMC_SAMPLE_LENGTH_SHIFT_AMOUNT = 4;

constexpr char APU_DMC_IRQ_ID[8] = "APU_DMC";


struct APUDMCParametersRegisterFlags {
	uint8_t rate : 4;
	uint8_t unused : 2;
	uint8_t loop : 1;
	uint8_t irqEnabled : 1;
};

union APUDMCParametersRegister {
	APUDMCParametersRegisterFlags flags;
	uint8_t value;
};

struct APUDMCState {
	APUDMCState();

	APUDMCParametersRegister paraemters;
	uint16_t sampleAddress;
	uint16_t sampleLength;

	uint16_t timer;
	bool irqFlag;

	uint8_t sampleBuffer;
	bool sampleBufferEmpty;
	uint16_t bytesRemaining;
	uint16_t currentSampleAddress;
	bool sampleNeeded;

	// Output unit
	uint8_t outputLevel;
	uint8_t outputShiftRegister;
	size_t shifterBitsRemaining;
	bool outputSilenceFlag;

	bool silenced;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& paraemters.value;
		ar& sampleAddress;
		ar& sampleLength;
		
		ar& timer;
		ar& irqFlag;

		ar& sampleBuffer;
		ar& sampleBufferEmpty;
		ar& bytesRemaining;
		ar& currentSampleAddress;
		ar& sampleNeeded;

		ar& outputLevel;
		ar& outputShiftRegister;
		ar& shifterBitsRemaining;
		ar& outputSilenceFlag;

		ar& silenced;
	}
};


class APUDMC {
public:
	APUDMC(CPU2A03* cpu);
	~APUDMC();

	void ConnectToBus(Bus* bus);
	void DisconnectFromBus();

	void SoftReset();
	void HardReset();

	void ClockTimer();
	void ResetTimer();

	uint8_t GetAudioSample();
	void SilenceChannel();
	void PlayChannel();

	void WriteParameters(uint8_t data);
	void WriteDirectLoad(uint8_t data);
	void WriteSampleAddress(uint8_t data);
	void WriteSampleLength(uint8_t data);

	bool IsDMCActive() const;
	bool IsInterruptAsserted() const;
	void ClearInterruptFlag();

	bool IsRequestingSample();

	APUDMCState GetState() const;
	void LoadState(APUDMCState& state);

private:
	void ClockOutputUnit();
	void OutputUnitNewCycle();
	void ChangeOutputLevel();
	void FillShiftRegisterFromSampleBuffer();
	void GetNewSampleIntoBuffer();
	void HandleZeroBytesRemaining();
	void StartPlay();


	CPU2A03* m_cpu;
	Bus* m_bus;

	APUDMCParametersRegister m_paraemters;
	uint16_t m_sampleAddress;
	uint16_t m_sampleLength;

	uint16_t m_timer;
	bool m_irqFlag;

	// Memory Reader
	uint8_t m_sampleBuffer;
	bool m_sampleBufferEmpty;
	uint16_t m_bytesRemaining;
	uint16_t m_currentSampleAddress;
	bool m_sampleNeeded;

	// Output unit
	uint8_t m_outputLevel;
	uint8_t m_outputShiftRegister;
	size_t m_shifterBitsRemaining;
	bool m_outputSilenceFlag;

	bool m_silenced;
};
