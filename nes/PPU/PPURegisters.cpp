#include "PPU2C02.h"

// PPUCTRL set to all 0's on reset
void PPUCTRLRegister::SoftReset() {
	value = 0;
}

void PPUCTRLRegister::HardReset() {
	value = 0;
}

// PPUMASK set to all 0's on reset
void PPUMASKRegister::SoftReset() {
	value = 0;
}

void PPUMASKRegister::HardReset() {
	value = 0;
}

// Reset behaviour of PPUSTATUS is complicated on NES. I'm simplifying it here
void PPUSTATUSRegister::SoftReset() {
	// VBlank should not change on soft reset
	flags.Unused = 0;
	flags.SpriteOverflow = 0;
	flags.Sprite0Hit = 0;
}

void PPUSTATUSRegister::HardReset() {
	// Some flags may actually be set during power on NES, but for emulation purposes it's better to set them all to 0
	value = 0xFF;
}


uint8_t PPU2C02::PPUCTRLRead() {
	// Cannot read from PPUCTRL, return latch value instead
	return m_latchValue;
}

void PPU2C02::PPUCTRLWrite(uint8_t data) {
	// On first frame, the register ignores reads/writes
	if (m_frameCount == 0) {
		// I assume the latch value should still be set
		m_latchValue = data;
		return;
	}

	bool oldNMIEnabled = m_PPUCTRL.flags.NMIEnabled;
	m_PPUCTRL.value = data;
	m_latchValue = data;

	// Generate NMI if VBLANK is set in PPUStatus and NMI changes to enabled
	if (m_PPUSTATUS.flags.VBlank && !oldNMIEnabled && m_PPUCTRL.flags.NMIEnabled) {
		m_cpu->RaiseNMI();
	}
}

uint8_t PPU2C02::PPUMASKRead() {
	// Cannot read from PPUMASK, return latch value instead
	return m_latchValue;
}

void PPU2C02::PPUMASKWrite(uint8_t data) {
	// On first frame, the register ignores reads/writes
	if (m_frameCount == 0) {
		// I assume the latch value should still be set
		m_latchValue = data;
		return;
	}

	// Disabling rendering outside VBLANK can corrupt OAM. TODO: Decide if that should be implemented
	m_PPUMASK.value = data;
	m_latchValue = data;
}

uint8_t PPU2C02::PPUSTATUSRead() {
	uint8_t valueToReturn = m_PPUSTATUS.value;

	// Remove unused bits
	valueToReturn = ClearLowerBits8(valueToReturn, 5);

	// Set latch values in unused bits
	valueToReturn |= ClearUpperBits8(m_latchValue, 5);

	// Clear VBlank, TODO: Clear PPUADDR and PPUSCROLL address latch
	// TODO: VBlank race condition. Reading PPUStatus approximately when it is set suppresses NMI for this frame
	m_PPUSTATUS.flags.VBlank = 0;
	m_latchValue = valueToReturn;

	return valueToReturn;
}

void PPU2C02::PPUSTATUSWrite(uint8_t data) {
	// Cannot write to PPUSTATUS, but still write to latch
	m_latchValue = data;
}

uint8_t PPU2C02::OAMADDRRead() {
	return 0;
}

void PPU2C02::OAMADDRWrite(uint8_t data) {
	
}

uint8_t PPU2C02::OAMDATARead() {
	return 0;
}

void PPU2C02::OAMDATAWrite(uint8_t data) {

}

uint8_t PPU2C02::PPUSCROLLRead() {
	return 0;
}

void PPU2C02::PPUSCROLLWrite(uint8_t data) {

}

uint8_t PPU2C02::PPUADDRRead() {
	return 0;
}

void PPU2C02::PPUADDRWrite(uint8_t data) {

}

uint8_t PPU2C02::PPUDATARead() {
	return 0;
}

void PPU2C02::PPUDATAWrite(uint8_t data) {

}