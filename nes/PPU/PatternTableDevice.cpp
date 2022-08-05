#include <assert.h>

#include "PatternTableDevice.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

PatternTableDevice::PatternTableDevice() : BusDevice(std::list({AddressRange(PATTERN_TABLE_BEGIN_ADDRESS, PATTERN_TABLE_END_ADDRESS)})) {
	m_cartridge = nullptr;
}

PatternTableDevice::~PatternTableDevice() {
	this->DisconnectCartridge();
}


// Nothing to do on reset/power up
void PatternTableDevice::SoftReset() {}
void PatternTableDevice::HardReset() {}

uint8_t PatternTableDevice::Read(uint16_t address) {
	return m_cartridge->PPURead(address);
}

void PatternTableDevice::Write(uint16_t address, uint8_t data) {
	m_cartridge->PPUWrite(address, data);
}

uint8_t PatternTableDevice::Probe(uint16_t address) {
	return m_cartridge->Probe(address);
}

PatternTableState PatternTableDevice::GetState() const {
	PatternTableState state;
	for (uint16_t address = PATTERN_TABLE_BEGIN_ADDRESS; address < PATTERN_TABLE_BEGIN_ADDRESS + PATTERN_TABLE_SIZE; address++) {
		state.patternTable0[address] = m_cartridge->ProbePPU(address);
	}
	for (uint16_t address = PATTERN_TABLE_BEGIN_ADDRESS + PATTERN_TABLE_SIZE; address < PATTERN_TABLE_END_ADDRESS; address++) {
		state.patternTable1[address - PATTERN_TABLE_SIZE] = m_cartridge->ProbePPU(address);
	}
	return state;
}

// Nothing to load. Except cartridge maybe?
void PatternTableDevice::LoadState(PatternTableState& state) {}

void PatternTableDevice::ConnectCartridge(Cartridge* cartridge) {
	assert(m_cartridge == nullptr);
	m_cartridge = cartridge;
}

void PatternTableDevice::DisconnectCartridge() {
	m_cartridge = nullptr;
}

uint16_t PatternTableDevice::GetTileLowBitsAddress(unsigned int patternTable, uint8_t tileID, uint8_t row) {
	return PATTERN_TABLE_BEGIN_ADDRESS + patternTable * PATTERN_TABLE_SIZE + tileID * 2 * PATTERN_TABLE_TILE_HEIGHT + row;
}

uint16_t PatternTableDevice::GetTileHighBitsAddress(unsigned int patternTable, uint8_t tileID, uint8_t row) {
	return PATTERN_TABLE_BEGIN_ADDRESS + patternTable * PATTERN_TABLE_SIZE + tileID * 2 * PATTERN_TABLE_TILE_HEIGHT + row + PATTERN_TABLE_MSB_OFFSET;
}

uint16_t PatternTableDevice::GetSpriteLowBitsAddress(unsigned int patternTable, bool mode8by16, uint8_t tileID, uint8_t spriteY, unsigned int scanline, bool flipVertically) {
	unsigned int row = scanline - spriteY;
	if (mode8by16) {
		if (flipVertically) {
			row = 2 * PATTERN_TABLE_TILE_HEIGHT - 1 - row;
		}
		return PatternTableDevice::GetSpriteLowBitsAddress8by16(tileID, row);
	}
	else {
		if (flipVertically) {
			row = PATTERN_TABLE_TILE_HEIGHT - 1 - row;
		}
		return PatternTableDevice::GetSpriteLowBitsAddress8by8(patternTable, tileID, row);
	}
}

uint16_t PatternTableDevice::GetSpriteHighBitsAddress(unsigned int patternTable, bool mode8by16, uint8_t tileID, uint8_t spriteY, unsigned int scanline, bool flipVertically) {
	unsigned int row = scanline - spriteY;
	if (mode8by16) {
		if (flipVertically) {
			row = 2 * PATTERN_TABLE_TILE_HEIGHT - 1 - row;
		}
		return PatternTableDevice::GetSpriteHighBitsAddress8by16(tileID, row);
	}
	else {
		if (flipVertically) {
			row = PATTERN_TABLE_TILE_HEIGHT - 1 - row;
		}
		return PatternTableDevice::GetSpriteHighBitsAddress8by8(patternTable, tileID, row);
	}
}

std::array<uint8_t, PATTERN_TABLE_TILE_WIDTH> PatternTableDevice::GetRowColourIndices(uint8_t rowLowBits, uint8_t rowHighBits) {
	std::array<uint8_t, PATTERN_TABLE_TILE_WIDTH> colours;
	for (unsigned int i = 0; i < PATTERN_TABLE_TILE_WIDTH; i++) {
		colours[i] = TestBit8(rowLowBits, 7) | ShiftLeft8(TestBit8(rowHighBits, 7), 1);
		rowLowBits = ShiftLeft8(rowLowBits, 1);
		rowHighBits = ShiftLeft8(rowHighBits, 1);
	}
	return colours;
}

uint16_t PatternTableDevice::GetSpriteLowBitsAddress8by8(unsigned int patternTable, uint8_t tileID, unsigned int row) {
	return PATTERN_TABLE_BEGIN_ADDRESS + patternTable * PATTERN_TABLE_SIZE + tileID * 2 * PATTERN_TABLE_TILE_HEIGHT + row;
}

uint16_t PatternTableDevice::GetSpriteLowBitsAddress8by16(uint8_t tileID, unsigned int row) {
	unsigned int patternTable = TestBit8(tileID, 0);
	// This selects top or bottom tile
	if (row >= PATTERN_TABLE_TILE_HEIGHT) {
		SetBit8(tileID, 0);
		row -= PATTERN_TABLE_TILE_HEIGHT;
	}
	else {
		ClearBit8(tileID, 0);
	}
	return PATTERN_TABLE_BEGIN_ADDRESS + patternTable * PATTERN_TABLE_SIZE + tileID * 2 * PATTERN_TABLE_TILE_HEIGHT + row;
}

uint16_t PatternTableDevice::GetSpriteHighBitsAddress8by8(unsigned int patternTable, uint8_t tileID, unsigned int row) {
	return PATTERN_TABLE_BEGIN_ADDRESS + patternTable * PATTERN_TABLE_SIZE + tileID * 2 * PATTERN_TABLE_TILE_HEIGHT + row + PATTERN_TABLE_MSB_OFFSET;
}

uint16_t PatternTableDevice::GetSpriteHighBitsAddress8by16(uint8_t tileID, unsigned int row) {
	unsigned int patternTable = TestBit8(tileID, 0);
	// This selects top or bottom tile
	if (row >= PATTERN_TABLE_TILE_HEIGHT) {
		SetBit8(tileID, 0);
		row -= PATTERN_TABLE_TILE_HEIGHT;
	}
	else {
		ClearBit8(tileID, 0);
	}
	return PATTERN_TABLE_BEGIN_ADDRESS + patternTable * PATTERN_TABLE_SIZE + tileID * 2 * PATTERN_TABLE_TILE_HEIGHT + row + PATTERN_TABLE_MSB_OFFSET;
}