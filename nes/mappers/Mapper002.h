#pragma once

#include "Cartridge.h"
#include "../ROM/INESFile.h"

constexpr uint16_t MAPPER_002_PRG_FIRST_BANK_START = PRG_ROM_START_ADDRESS;
constexpr uint16_t MAPPER_002_PRG_BANK_SIZE = 0x4000;
constexpr uint16_t MAPPER_002_CHR_FIRST_BANK_START = 0x0;
constexpr uint16_t MAPPER_002_CHR_BANK_SIZE = 0x2000;
constexpr unsigned int MAPPER_002_STANDARD_NUM_BANKS = 8;
constexpr unsigned int MAPPER_002_BANK_SELECT_STANDARD_HIGH_BIT = 2;
constexpr unsigned int MAPPER_002_BANK_SELECT_NON_STANDARD_HIGH_BIT = 3;


struct Mapper002State : public MapperAdditionalState {};

class Mapper002 : public Cartridge {
public:
	Mapper002(const INESFile& romFile);

	void SetupLogicalBanks() override;
	void InitializeBankMapping() override;

	MirroringMode GetCurrentMirroringMode() override;

private:
	void GetAdditionalState(std::shared_ptr<MapperAdditionalState> state) const;
	void LoadAdditionalState(std::shared_ptr<MapperAdditionalState> state);

	void PRGROMWrite(uint16_t address, uint8_t data);

	MirroringMode m_mirroringMode;
	size_t m_numPRGROMBanks;
};