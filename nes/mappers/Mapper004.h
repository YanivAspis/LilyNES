#pragma once

#include "Cartridge.h"
#include "../ROM/INESFile.h"
#include "../CPU/CPU2A03.h"

constexpr size_t MAPPER_004_NUM_PRG_LOGICAL_BANKS = 4;
constexpr size_t MAPPER_004_NUM_CHR_LOGICAL_BANKS = 8;
constexpr uint16_t MAPPER_004_PRG_FIRST_BANK_START = 0x8000;
constexpr uint16_t MAPPER_004_CHR_FIRST_BANK_START = 0x0;
constexpr uint16_t MAPPER_004_PRG_BANK_SIZE = 0x2000;
constexpr uint16_t MAPPER_004_CHR_BANK_SIZE = 0x400;

constexpr uint16_t MAPPER_004_BANK_SELECT_WRITE_START_ADDRESS = 0x8000;
constexpr uint16_t MAPPER_004_BANK_SELECT_WRITE_END_ADDRESS = 0x9FFF;
constexpr uint16_t MAPPER_004_MIRRORING_PRGRAM_WRITE_START_ADDRESS = 0xA000;
constexpr uint16_t MAPPER_004_MIRRORING_PRGRAM_WRITE_END_ADDRESS = 0xBFFF;
constexpr uint16_t MAPPER_004_IRQ_LATCH_WRITE_START_ADDRESS = 0xC000;
constexpr uint16_t MAPPER_004_IRQ_LATCH_WRITE_END_ADDRESS = 0xDFFF;
constexpr uint16_t MAPPER_004_IRQ_ENABLE_WRITE_START_ADDRESS = 0xE000;
constexpr uint16_t MAPPER_004_IRQ_ENABLE_WRITE_END_ADDRESS = 0xFFFF;

constexpr size_t MAPPER_004_NUM_REGISTERS = 8;
constexpr uint8_t MAPPER_004_IRQ_COUNTER_INITIAL_VALUE = 0xFF;
constexpr unsigned int MAPPER_004_PATTERN_TABLE_TEST_BIT = 12;
constexpr char MAPPER_004_IRQ_ID[5] = "MMC3";


struct BankSelectRegisterFlags {
	uint8_t registerSelect : 3;
	uint8_t unused : 2;
	uint8_t MMC6PRGRAMEnable : 1; // Will do nothing in this implementation
	uint8_t PRGROMBankMode : 1;
	uint8_t CHRROMBankMode : 1;
};

union BankSelectRegister {
	BankSelectRegisterFlags flags;
	uint8_t value;
};

struct Mapper004State : public MapperAdditionalState {
	Mapper004State();
	MirroringMode mirroringMode;
	BankSelectRegister bankSelectRegister;

	bool A12State;
	uint8_t irqCounter;
	uint8_t irqLatchValue;
	bool irqReload;
	bool irqEnabled;
};

class Mapper004 : public Cartridge {
public:
	Mapper004(const INESFile& romFile, CPU2A03* cpu);
	~Mapper004();

	void SoftReset() override;
	void HardReset() override;

	void SetupLogicalBanks() override;
	void InitializeBankMapping() override;

	MirroringMode GetCurrentMirroringMode() override;

	uint8_t ProbePPU(uint16_t address) override;

private:
	std::any GetAdditionalState() const;
	void LoadAdditionalState(std::any state);

	void PRGROMWrite(uint16_t address, uint8_t data);
	void BankSelectWrite(uint8_t data);
	void BankDataWrite(uint8_t data);
	void MirroringWrite(uint8_t data);
	void PRGRAMProtectWrite(uint8_t data);
	void IRQLatchWrite(uint8_t data);
	void IRQReloadWrite();
	void IRQDisableWrite();
	void IRQEnableWrite();

	uint8_t CHRROMRead(uint16_t address) override;
	void CHRROMWrite(uint16_t address, uint8_t data) override;

	void DecrementIRQCounter(); // Called when A12 changes from 0 to 1

	CPU2A03* m_cpu;
	bool m_batteryBackedRAM;

	MirroringMode m_mirroringMode;
	size_t m_numPRGROMBanks;
	BankSelectRegister m_bankSelectRegister;
	
	bool m_A12State; // 0 - reading BG info (pattern table 0). 1 - Reading sprite info (pattern table 1)
	uint8_t m_irqCounter;
	uint8_t m_irqLatchValue;
	bool m_irqReload;
	bool m_irqEnabled;
};
