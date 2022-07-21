#include "PPU2C02.h"

// TODO: Used for generating static. Remove when we do actual rendering
#include <random>


PPU2C02::PPU2C02(Environment* enviroment, CPU2A03* cpu) : BusDevice(std::list<AddressRange>({AddressRange(PPU_ADDRESS_RANGE_BEGIN, PPU_ADDRESS_RANGE_END)})), m_environment(enviroment) {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;
	m_cpu = cpu;
	ResetNESPicture(m_picture);

	// TODO: Used for generating static. Remove when we do actual rendering
	srand(time(nullptr));
}

void PPU2C02::SoftReset() {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;
	ResetNESPicture(m_picture);
}

void PPU2C02::HardReset() {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;
	ResetNESPicture(m_picture);
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
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_int_distribution<int> rng(0, 1);

	if (m_scanline >= PPU_VISIBLE_SCANLINES_BEGIN && m_scanline < PPU_VISIBLE_SCANLINES_END && m_dot >= PPU_VISIBLE_DOT_BEGIN && m_dot < PPU_VISIBLE_DOT_END) {
		int val = rng(mt);
		m_picture[m_scanline][m_dot-1].red = 255 * val;
		m_picture[m_scanline][m_dot-1].green = 255 * val;
		m_picture[m_scanline][m_dot-1].blue = 255 * val;
	}
	if (m_scanline == PPU_VISIBLE_SCANLINES_END && m_dot == 0) {
		if (m_environment != nullptr) {
			m_environment->UpdateDisplay(m_picture);
		}
	}
	
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