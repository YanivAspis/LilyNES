#include "PPU2C02.h"


PPU2C02::PPU2C02(CPU2A03* cpu) : BusDevice(std::list<AddressRange>({AddressRange(PPU_ADDRESS_RANGE_BEGIN, PPU_ADDRESS_RANGE_END)})) {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;
	m_cpu = cpu;
}

void PPU2C02::SoftReset() {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;
}

void PPU2C02::HardReset() {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;
}

uint8_t PPU2C02::Read(uint16_t address) {
	return 0;
}

void PPU2C02::Write(uint16_t address, uint8_t data) {

}

uint8_t PPU2C02::Probe(uint16_t address) {
	return 0;
}

PPUState PPU2C02::GetState() const {
	PPUState state;
	state.frameCount = m_frameCount;
	state.scanline = m_scanline;
	state.dot = m_dot;
	return state;
}

void PPU2C02::LoadState(PPUState& state) {
	m_frameCount = state.frameCount;
	m_scanline = state.scanline;
	m_dot = state.dot;
}

void PPU2C02::Clock() {
	/*
	if (m_scanline == PPU_NMI_SCANLINE && m_dot == PPU_NMI_DOT) {
		m_cpu->RaiseNMI();
	}*/
	this->IncrementDotScanline();
}

unsigned int PPU2C02::GetFrameCount() const {
	return m_frameCount;
}

void PPU2C02::IncrementDotScanline() {
	m_dot++;
	if (m_dot == PPU_NUM_DOTS_PER_SCANLINE) {
		m_dot = 0;
		m_scanline++;
	}
	if (m_scanline == PPU_NUM_SCANLINES) {
		m_scanline = 0;
		m_frameCount++;

		// Skip (0,0) cycle on odd frames
		if (m_frameCount % 2 == 1) {
			m_dot++;
		}
	}
}