#pragma once

#include "../BusDevice.h"

constexpr uint16_t OAMDMA_ADDRESS = 0x4014;
constexpr uint16_t OAMDATA_ADDRESS = 0x2004;
constexpr unsigned int OAMDMA_CYCLES_EVEN = 513;
constexpr unsigned int OAMDMA_CYCLES_ODD = 514;
constexpr unsigned int OAMDMA_TRANSFER_BEGIN_CYCLE = 512;

struct OAMDMAState {
	uint8_t page;
	uint8_t readAddressLow;
	uint8_t loadedOAMData;
	unsigned int cyclesRemaining;
	unsigned int cpuCycleCount;
};

class OAMDMADevice : public BusDevice {
public:
	OAMDMADevice();

	void SoftReset() override;
	void HardReset() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	OAMDMAState GetState() const;
	void LoadState(OAMDMAState& state);

	void Clock();

	unsigned int GetCyclesRemaining() const;

private:
	uint8_t m_page;
	uint8_t m_readAddressLow;
	uint8_t m_loadedOAMData;
	unsigned int m_cyclesRemaining;
	unsigned int m_cpuCycleCount;
};
