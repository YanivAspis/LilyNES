#pragma once

#include "Cartridge.h"
#include "../ROM/INESFile.h"

constexpr uint16_t MAPPER_000_PRG_FIRST_BANK_START = PRG_ROM_START_ADDRESS;
constexpr uint16_t MAPPER_000_PRG_BANK_SIZE = 0x4000;
constexpr uint16_t MAPPER_000_CHR_FIRST_BANK_START = 0x0;
constexpr uint16_t MAPPER_000_CHR_BANK_SIZE = 0x2000;

//struct Mapper000State : public MapperAdditionalState {};

class Mapper000 : public Cartridge {
public:
	Mapper000(const INESFile& romFile);

	void SetupLogicalBanks() override;
	void InitializeBankMapping() override;

	MirroringMode GetCurrentMirroringMode() override;

private:
	std::vector<uint8_t> GetAdditionalState() const;
	void LoadAdditionalState(const std::vector<uint8_t>& state);

	void PRGROMWrite(uint16_t address, uint8_t data);

	MirroringMode m_mirroringMode;
	size_t m_numPRGROMBanks;
};
