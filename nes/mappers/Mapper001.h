#pragma once

#include "Cartridge.h"
#include "../ROM/INESFile.h"

/*
	This is a naive implementation of Mapper 001 (SxROM) that won't correctly emulate all games.
	Hopefully it emulates all games I care about correctly.
	Assumptions: CHR Lines only select CHR-ROM/RAM banks
				 PRG RAM size is always 8 KB (no PRG RAM bank selection)
				 PRG RAM always enabled
				 Writes to PRG ROM on consecutive cycles never happen
*/

constexpr uint16_t MAPPER_001_PRG_FIRST_BANK_START = PRG_ROM_START_ADDRESS;
constexpr uint16_t MAPPER_001_PRG_BANK_SIZE = 0x4000;
constexpr uint16_t MAPPER_001_CHR_FIRST_BANK_START = 0x0;
constexpr uint16_t MAPPER_001_CHR_BANK_SIZE = 0x1000;

constexpr unsigned int MAPPER_001_REGISTER_SELECT_LOW_BIT = 13;
constexpr unsigned int MAPPER_001_REGISTER_SELECT_HIGH_BIT = 14;
constexpr uint8_t MAPPER_001_CONTROL_SELECT = 0;
constexpr uint8_t MAPPER_001_CHR_BANK_0_SELECT = 1;
constexpr uint8_t MAPPER_001_CHR_BANK_1_SELECT = 2;
constexpr uint8_t MAPPER_001_PRG_BANK_SELECT = 3;

constexpr unsigned int MAPPER_001_LOAD_REGISTER_RESET_BIT = 7;
constexpr unsigned int MAPPER_001_LOAD_REGISTER_DATA_BIT = 0;
constexpr unsigned int MAPPER_001_LOAD_REGISTER_NUM_WRITES = 5;

enum Mapper001MirroringMode {
	MAPPER_001_MIRRORING_SINGLE_SCREEN_LOW,
	MAPPER_001_MIRRORING_SINGLE_SCREEN_HIGH,
	MAPPER_001_MIRRORING_VERTICAL,
	MAPPER_001_MIRRORING_HORIZONTAL,
};

enum Mapper001PRGBankMode {
	MAPPER_001_PRG_BANK_32K_0,
	MAPPER_001_PRG_BANK_32K_1, // Same effect as MAPPER_001_PRG_BANK_32K_0
	MAPPER_001_PRG_BANK_16K_LOW_FIXED,
	MAPPER_001_PRG_BANK_16K_HIGH_FIXED,
};

enum Mapper001CHRBankMode {
	MAPPER_001_CHR_BANK_8K,
	MAPPER_001_CHR_BANK_4K,
};

struct ControlRegisterFlags {
	uint8_t mirroring : 2;
	uint8_t PRGBankMode : 2;
	uint8_t CHRBankMode : 1;
	uint8_t unused : 3;
};

union ControlRegister {
	ControlRegisterFlags flags;
	uint8_t value;
};

struct Mapper001State {
	Mapper001State();
	uint8_t loadData;
	unsigned int loadCounter;
	ControlRegister controlRegister;
};

class Mapper001 : public Cartridge {
public:
	Mapper001(const INESFile& romFile);

	void SoftReset() override;
	void HardReset() override;

	void SetupLogicalBanks() override;
	void InitializeBankMapping() override;

	MirroringMode GetCurrentMirroringMode() override;

private:
	std::vector<uint8_t> GetAdditionalState() const;
	void LoadAdditionalState(const std::vector<uint8_t>& state);

	void PRGROMWrite(uint16_t address, uint8_t data);

	void RegisterWrite(uint16_t address);
	void ControlRegisterWrite();
	void CHRBank0Write();
	void CHRBank1Write();
	void PRGBankWrite();
	void ResetLoadRegister();

	size_t m_numPRGROMBanks;
	//size_t m_numCHRROMBanks;
	//bool m_batteryBackedRAM;

	uint8_t m_loadData;
	unsigned int m_loadCounter; // Count 5 writes to PRG ROM before making changes
	ControlRegister m_controlRegister;
};
