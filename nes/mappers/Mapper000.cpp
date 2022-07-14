#include "Mapper000.h"
#include "../../BitwiseUtils.h"
using namespace BitwiseUtils;

Mapper000::Mapper000(const INESFile& romFile) 
: Cartridge(romFile)
{
	m_numPRGROMBanks = m_PRGROM.size() / PRG_ROM_BANK_SIZE;
	m_mirroringMode = (romFile.GetHeader().GetFixedMirroringMode() == FIXED_MIRROR_HORIZONTAL) ? MIRRORING_HORIZONTAL : MIRRORING_VERTICAL;
}

uint8_t Mapper000::Read(uint16_t address) {
	if (address >= PRG_RAM_START_ADDRESS && address <= PRG_RAM_END_ADDRESS) {
		return this->PRGRAMRead(address);
	}
	else if (address >= PRG_ROM_START_ADDRESS && address <= PRG_ROM_END_ADDRESS) {
		return this->PRGROMRead(address);
	}

	// Not supposed to reach here
	return 0;
}

void Mapper000::Write(uint16_t address, uint8_t data) {
	if (address >= PRG_RAM_START_ADDRESS && address <= PRG_RAM_END_ADDRESS) {
		this->PRGRAMWrite(address, data);
	}
	else if (address >= PRG_ROM_START_ADDRESS && address <= PRG_ROM_END_ADDRESS) {
		this->PRGROMWrite(address, data);
	}
}


uint8_t Mapper000::PPURead(uint16_t address) {
	// Mirror to Pattern Table range
	address = ClearUpperBits16(address, 13);
	return m_CHRROM[address];
}

void Mapper000::PPUWrite(uint16_t address, uint8_t data) {
	// CHRRAM not supported for this mapper, ignore
	return;
}

MirroringMode Mapper000::GetCurrentMirroringMode() {
	return m_mirroringMode;
}


void Mapper000::GetAdditionalState(std::shared_ptr<MapperAdditionalState> state) const {}

void Mapper000::LoadAdditionalState(std::shared_ptr<MapperAdditionalState> state) {}


uint8_t Mapper000::PRGROMRead(uint16_t address) {
	address -= PRG_ROM_START_ADDRESS;

	// If single bank (16KB mode), mirror address to 0x8000-0xBFFF
	if (m_numPRGROMBanks == 1) {
		address = BitwiseUtils::ClearUpperBits16(address, 14);
	}

	return m_PRGROM[address];
}

void Mapper000::PRGROMWrite(uint16_t address, uint8_t data) {
	// Can't write to ROM, ingore
	return;
}

uint8_t Mapper000::PRGRAMRead(uint16_t address) {
	address -= PRG_RAM_START_ADDRESS;

	// If address out of range, return 0? 
	// Or perhaps open bus behaviour should be implemented?
	if (address < (uint16_t)m_PRGRAM.size()) {
		return m_PRGRAM[address];
	}
	else {
		return 0;
	}
}

void Mapper000::PRGRAMWrite(uint16_t address, uint8_t data) {
	address -= PRG_RAM_START_ADDRESS;

	// If address out of range, ignore
	if (address < (uint16_t)m_PRGRAM.size()) {
		m_PRGRAM[address] = data;
	}
}