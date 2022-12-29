#pragma once

#include "Cartridge.h"
#include "../ROM/INESFile.h"

constexpr uint16_t MAPPER_003_PRG_FIRST_BANK_START = PRG_ROM_START_ADDRESS;
constexpr uint16_t MAPPER_003_PRG_BANK_SIZE = 0x4000;
constexpr uint16_t MAPPER_003_CHR_FIRST_BANK_START = 0x0;
constexpr uint16_t MAPPER_003_CHR_BANK_SIZE = 0x2000;
constexpr unsigned int MAPPER_003_STANDARD_NUM_BANKS = 4;
constexpr unsigned int MAPPER_003_BANK_SELECT_STANDARD_HIGH_BIT = 1;

//struct Mapper003State : public MapperAdditionalState {};

class Mapper003 : public Cartridge {
public:
	Mapper003(const INESFile& romFile);

	void SetupLogicalBanks() override;
	void InitializeBankMapping() override;

	MirroringMode GetCurrentMirroringMode() override;

private:
	std::vector<uint8_t> GetAdditionalState() const;
	void LoadAdditionalState(const std::vector<uint8_t>& state);

	void PRGROMWrite(uint16_t address, uint8_t data);

	MirroringMode m_mirroringMode;
	size_t m_numPRGROMBanks;
	size_t m_numCHRROMBanks;
};