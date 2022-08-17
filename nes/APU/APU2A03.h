#pragma once

#include "../BusDevice.h"

constexpr uint16_t APU_BEGIN_ADDRESS_1 = 0x4000;
constexpr uint16_t APU_END_ADDRESS_1 = 0x4013;
constexpr uint16_t APU_BEGIN_ADDRESS_2 = 0x4015;
constexpr uint16_t APU_END_ADDRESS_2 = 0x4015;


struct APUState {

};

class APU2A03 : public BusDevice {
public:
	APU2A03();

	void SoftReset() override;
	void HardReset() override;

	void Read(uint16_t address, uint8_t& data) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	float GetAudioSample();

	APUState GetState() const;
	void LoadState(APUState& state);

private:

};