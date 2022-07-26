#include "PPU2C02.h"
#include "PaletteRAMDevice.h"

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
	flags.unused = 0;
	flags.spriteOverflow = 0;
	flags.sprite0Hit = 0;
}

void PPUSTATUSRegister::HardReset() {
	// Some flags may actually be set during power on NES, but for emulation purposes it's better to set them all to 0
	value = 0;
}


uint8_t PPU2C02::PPUCTRLRead() {
	// Cannot read from PPUCTRL, return latch value instead
	return m_latchValue;
}

void PPU2C02::PPUCTRLWrite(uint8_t data) {
	// On first frame, the register ignores reads/writes
	if (m_frameCount == 0) {
		// I assume the latch value should still be set
		this->SetLatchValue(data);
		return;
	}

	bool oldNMIEnabled = m_PPUCTRL.flags.NMIEnabled;
	m_PPUCTRL.value = data;
	// Set nametable values in loopy T register
	m_TRAMAddress.scrollFlags.nametableX = m_PPUCTRL.flags.nametableX;
	m_TRAMAddress.scrollFlags.nametableY = m_PPUCTRL.flags.nametableY;
	this->SetLatchValue(data);

	// Generate NMI if VBLANK is set in PPUStatus and NMI is changed from 0 to 1
	// This can cause several NMIs to be generated in a single frame. Some games do that
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
		this->SetLatchValue(data);
		return;
	}

	// Disabling rendering outside VBLANK can corrupt OAM. TODO: Decide if that should be implemented
	m_PPUMASK.value = data;
	this->SetLatchValue(data);
}

uint8_t PPU2C02::PPUSTATUSRead() {
	uint8_t valueToReturn = m_PPUSTATUS.value;

	// Remove unused bits
	valueToReturn = ClearLowerBits8(valueToReturn, 5);

	// Set latch values in unused bits
	valueToReturn |= ClearUpperBits8(m_latchValue, 5);

	// Clear VBlank, and for some reason, the loopy write toggle
	// TODO: VBlank race condition. Reading PPUStatus approximately when it is set suppresses NMI for this frame
	m_PPUSTATUS.flags.VBlank = 0;
	m_loopyWriteToggle = false;
	this->SetLatchValue(valueToReturn);

	return valueToReturn;
}

void PPU2C02::PPUSTATUSWrite(uint8_t data) {
	// Cannot write to PPUSTATUS, but still write to latch
	this->SetLatchValue(data);
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
	// Write only register, so return latch value
	return m_latchValue;
}

void PPU2C02::PPUSCROLLWrite(uint8_t data) {
	// On first frame, the register ignores reads/writes
	if (m_frameCount == 0) {
		// I assume the latch value should still be set
		this->SetLatchValue(data);
		return;
	}

	if (m_loopyWriteToggle) {
		// Second write - Y scroll position
		m_TRAMAddress.scrollFlags.coarseY = ShiftRight8(ClearLowerBits8(data, 3), 3);
		m_TRAMAddress.scrollFlags.fineY = ClearUpperBits8(data, 3);
		m_loopyWriteToggle = false;
	}
	else {
		// First write - X scroll position
		m_TRAMAddress.scrollFlags.coarseX = ShiftRight8(ClearLowerBits8(data, 3), 3);
		m_fineX = ClearUpperBits8(data, 3);
		m_loopyWriteToggle = true;
	}
	this->SetLatchValue(data);
}

uint8_t PPU2C02::PPUADDRRead() {
	// Write only register, so return latch value
	return m_latchValue;
}

void PPU2C02::PPUADDRWrite(uint8_t data) {
	// On first frame, the register ignores reads/writes
	if (m_frameCount == 0) {
		// I assume the latch value should still be set
		this->SetLatchValue(data);
		return;
	}

	// TODO: Bus conflict when writing to this register during raster effects?
	if (m_loopyWriteToggle) {
		// Second write - LSB written to T, T copied to V
		m_TRAMAddress.address = ClearLowerBits16(m_TRAMAddress.address, 8);
		m_TRAMAddress.address |= (uint16_t)data;
		m_VRAMAddress.address = m_TRAMAddress.address;
		m_loopyWriteToggle = false;
	}
	else {
		// First write - MSB written to T
		m_TRAMAddress.address = ClearUpperBits16(m_TRAMAddress.address, 8);
		// Top two bits unused - they should be 0 in MSB of T
		data = ClearUpperBits8(data, 6);
		m_TRAMAddress.address |= ShiftLeft16((uint16_t)data, 8);
		m_loopyWriteToggle = true;
	}

	this->SetLatchValue(data);
}

uint8_t PPU2C02::PPUDATARead() {
	// Get VRAM address and mirror to 0x0000 - 0x3FFF
	 uint16_t address = ClearUpperBits16(m_VRAMAddress.address, 14);
	 uint8_t valueToReturn = 0;

	 if (m_VRAMAddress.address >= PALETTE_RAM_BEGIN_ADDRESS && m_VRAMAddress.address <= PALETTE_RAM_END_ADDRESS) {
		 // In Palette RAM - return value immediately and set internal buffer to nametable data
		 // "underneath" - basically imagine the nametable mirror was 0x3000 - 0x3FFF (instead of
		 // 0x3000 - 0x3EFF), and set to internal buffer to what would have been there.
		 // In practice, that's the content of address - 0x1000
		 m_PPUDATABuffer = m_ppuBus->Read(address - 0x1000);
		 uint8_t valueToReturn = m_ppuBus->Read(address);
		 
	 }
	 else {
		 // All other addresses - return content of internal buffer, set internal buffer to content of address
		 uint8_t valueToReturn = m_PPUDATABuffer;
		 m_PPUDATABuffer = m_ppuBus->Read(address);
	 }

	 // VRAM address is incremented for convenience
	 this->PPUDATAAddressIncrement();
	 this->SetLatchValue(valueToReturn);
	 return valueToReturn;
}

void PPU2C02::PPUDATAWrite(uint8_t data) {
	this->SetLatchValue(data);

	// Get VRAM address and mirror to 0x0000 - 0x3FFF
	uint16_t address = ClearUpperBits16(m_VRAMAddress.address, 14);

	// Unlike write, there is no buffering of data - it's written immediately
	m_ppuBus->Write(address, data);

	// VRAM address is incremented for convenience
	this->PPUDATAAddressIncrement();
}

void PPU2C02::PPUDATAAddressIncrement() {
	if (this->IsRendering()) {
		// If we are rendering, increment both y and coarse x
		// Most games don't do this since it corrupts graphics data, 
		// but Zelda II does this on the title screen
		this->IncrementCoarseX();
		this->IncrementY();
	}
	else {
		// Outside of rendering increment address according to the flag in PPUCTRL
		if (m_PPUCTRL.flags.incrementMode) {
			m_VRAMAddress.address += PPU_ADDRESS_INCREMENT_MODE_ON;
		}
		else {
			m_VRAMAddress.address += PPU_ADDRESS_INCREMENT_MODE_OFF;
		}
		// Deal with overflow? I'm not sure if bit 14 should be cleared here, or kept
		// But bit 15 obviously should be thrown out
		ClearBit16(m_VRAMAddress.address, 15);
	}
}

void PPU2C02::SetLatchValue(uint8_t latchValue) {
	m_latchValue = latchValue;
	m_latchCounter = PPU_LATCH_DECAY_CYCLES;
}

void PPU2C02::DecrementLatchCounter() {
	if (m_latchCounter == 0) {
		m_latchValue = 0;
	}
	else {
		m_latchCounter--;
	}
}