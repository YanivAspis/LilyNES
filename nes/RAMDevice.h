#pragma once

#include "BusDevice.h"
#include <array>

constexpr uint16_t RAM_ADDRESS_START = 0x0;
constexpr uint16_t RAM_PHYSICAL_SIZE = 0x800;
constexpr uint16_t RAM_NUM_MIRRORS = 4;
constexpr uint16_t RAM_ADDRESS_END = RAM_ADDRESS_START + RAM_NUM_MIRRORS * RAM_PHYSICAL_SIZE - 1;

struct RAMState {
	RAMState();
	std::array<uint8_t, RAM_PHYSICAL_SIZE> content;
};

class RAMDevice : public BusDevice {
public:
	RAMDevice();

	void SoftReset() override;
	void HardReset() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	RAMState GetState() const;
	void LoadState(RAMState& state);

private:
	std::array<uint8_t, RAM_PHYSICAL_SIZE> m_RAMContent;
};
