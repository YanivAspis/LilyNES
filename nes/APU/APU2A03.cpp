#include "APU2A03.h"

APU2A03::APU2A03() : BusDevice(std::list<AddressRange>({AddressRange(APU_BEGIN_ADDRESS_1, APU_END_ADDRESS_1), AddressRange(APU_BEGIN_ADDRESS_2, APU_END_ADDRESS_2) }))
{
}

void APU2A03::SoftReset()
{
}

void APU2A03::HardReset()
{
}

void APU2A03::Read(uint16_t address, uint8_t& data)
{
	data = 0;
}

void APU2A03::Write(uint16_t address, uint8_t data)
{
}

uint8_t APU2A03::Probe(uint16_t address)
{
	return 0;
}

float APU2A03::GetAudioSample()
{
	static double globalTime = 0;
	float sample = 1.0 * sin(2 * 3.14159265359 * 440.0 * globalTime);
	globalTime += 1.0 / 44100;
	return sample;
}

APUState APU2A03::GetState() const
{
	return APUState();
}

void APU2A03::LoadState(APUState& state)
{
}
