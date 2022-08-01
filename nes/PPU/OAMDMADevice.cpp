#include "OAMDMADevice.h"
#include "../../utils/BitwiseUtils.h"
#include "../Bus.h"

using namespace BitwiseUtils;

OAMDMADevice::OAMDMADevice() : BusDevice(std::list<AddressRange>({AddressRange(OAMDMA_ADDRESS, OAMDMA_ADDRESS)})) {
	m_page = 0;
	m_readAddressLow = 0;
	m_loadedOAMData = 0;
	m_cyclesRemaining = 0;
	m_cpuCycleCount = 0;
}

void OAMDMADevice::SoftReset() {
	m_page = 0;
	m_readAddressLow = 0;
	m_loadedOAMData = 0;
	m_cyclesRemaining = 0;
	m_cpuCycleCount = 0;
}

void OAMDMADevice::HardReset() {
	m_page = 0;
	m_readAddressLow = 0;
	m_loadedOAMData = 0;
	m_cyclesRemaining = 0;
	m_cpuCycleCount = 0;
}

uint8_t OAMDMADevice::Read(uint16_t address) {
	// TODO: Should return open-bus content, I guess, but I don't have access to these
	return 0;
}

void OAMDMADevice::Write(uint16_t address, uint8_t data) {
	m_page = data;
	m_cyclesRemaining = (m_cpuCycleCount % 2 == 0) ? OAMDMA_CYCLES_EVEN : OAMDMA_CYCLES_ODD;
}

uint8_t OAMDMADevice::Probe(uint16_t address) {
	return 0;
}

OAMDMAState OAMDMADevice::GetState() const {
	OAMDMAState state;
	state.page = m_page;
	state.readAddressLow = m_readAddressLow;
	state.loadedOAMData = m_loadedOAMData;
	state.cyclesRemaining = m_cyclesRemaining;
	state.cpuCycleCount = m_cpuCycleCount;
	return state;
}

void OAMDMADevice::LoadState(OAMDMAState& state) {
	m_page = state.page;
	m_readAddressLow = state.readAddressLow;
	m_loadedOAMData = state.loadedOAMData;
	m_cyclesRemaining = state.cyclesRemaining;
	m_cpuCycleCount = state.cpuCycleCount;
}

void OAMDMADevice::Clock() {
	m_cpuCycleCount++;

	if (m_cyclesRemaining == 0) {
		// OAM DMA transfer not active
		return;
	}

	// Performing OAM DMA transfer
	if (m_cyclesRemaining > OAMDMA_TRANSFER_BEGIN_CYCLE) {
		// Idle cycle
		m_cyclesRemaining--;
		return;
	}

	if (m_cyclesRemaining % 2 == 0) {
		// Read cycle
		m_loadedOAMData = m_bus->Read(CombineBytes(m_readAddressLow, m_page));
		Inc8Bit(m_readAddressLow);
	}
	else {
		m_bus->Write(OAMDATA_ADDRESS, m_loadedOAMData);
	}
	m_cyclesRemaining--;
}

unsigned int OAMDMADevice::GetCyclesRemaining() const {
	return m_cyclesRemaining;
}