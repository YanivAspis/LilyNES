#pragma once

#include "Cartridge.h"
#include "../ROM/INESFile.h"
#include "../CPU/CPU2A03.h"

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

constexpr uint16_t MAPPER_004_PRGROM_MODE0_REGISTER_6_START = 0x8000;
constexpr uint16_t MAPPER_004_PRGROM_MODE0_REGISTER_6_END = 0x9FFF;
constexpr uint16_t MAPPER_004_PRGROM_MODE0_REGISTER_7_START = 0xA000;
constexpr uint16_t MAPPER_004_PRGROM_MODE0_REGISTER_7_END = 0xBFFF;
constexpr uint16_t MAPPER_004_PRGROM_MODE0_LOW_FIXED_START = 0xC000;
constexpr uint16_t MAPPER_004_PRGROM_MODE0_LOW_FIXED_END = 0xDFFF;
constexpr uint16_t MAPPER_004_PRGROM_MODE0_HIGH_FIXED_START = 0xE000;
constexpr uint16_t MAPPER_004_PRGROM_MODE0_HIGH_FIXED_END = 0xFFFF;

constexpr uint16_t MAPPER_004_PRGROM_MODE1_LOW_FIXED_START = 0x8000;
constexpr uint16_t MAPPER_004_PRGROM_MODE1_LOW_FIXED_END = 0x9FFF;
constexpr uint16_t MAPPER_004_PRGROM_MODE1_REGISTER_7_START = 0xA000;
constexpr uint16_t MAPPER_004_PRGROM_MODE1_REGISTER_7_END = 0xBFFF;
constexpr uint16_t MAPPER_004_PRGROM_MODE1_REGISTER_6_START = 0xC000;
constexpr uint16_t MAPPER_004_PRGROM_MODE1_REGISTER_6_END = 0xDFFF;
constexpr uint16_t MAPPER_004_PRGROM_MODE1_HIGH_FIXED_START = 0xE000;
constexpr uint16_t MAPPER_004_PRGROM_MODE1_HIGH_FIXED_END = 0xFFFF;

constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_0_START = 0x0;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_0_END = 0x7FF;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_1_START = 0x800;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_1_END = 0xFFF;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_2_START = 0x1000;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_2_END = 0x13FF;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_3_START = 0x1400;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_3_END = 0x17FF;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_4_START = 0x1800;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_4_END = 0x1BFF;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_5_START = 0x1C00;
constexpr uint16_t MAPPER_004_CHRROM_MODE0_REGISTER_5_END = 0x1FFF;

constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_2_START = 0x0;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_2_END = 0x3FF;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_3_START = 0x400;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_3_END = 0x7FF;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_4_START = 0x800;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_4_END = 0xBFF;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_5_START = 0xC00;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_5_END = 0xFFF;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_0_START = 0x1000;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_0_END = 0x17FF;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_1_START = 0x1800;
constexpr uint16_t MAPPER_004_CHRROM_MODE1_REGISTER_1_END = 0x1FFF;


constexpr size_t MAPPER_004_NUM_REGISTERS = 8;
constexpr uint8_t MAPPER_004_IRQ_COUNTER_INITIAL_VALUE = 0xFF;
constexpr unsigned int MAPPER_004_PATTERN_TABLE_TEST_BIT = 12;

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
	std::array<uint8_t, MAPPER_004_NUM_REGISTERS> registers;

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

	uint8_t PRGROMRead(uint16_t address) override;
	uint8_t PRGROMReadMode0(uint16_t address);
	uint8_t PRGROMReadMode1(uint16_t address);

	uint8_t CHRROMRead(uint16_t address) override;
	uint8_t CHRROMReadMode0(uint16_t address);
	uint8_t CHRROMReadMode1(uint16_t address);

	void CHRROMWrite(uint16_t address, uint8_t data) override;
	void CHRROMWriteMode0(uint16_t address, uint8_t data);
	void CHRROMWriteMode1(uint16_t address, uint8_t data);

	void DecrementIRQCounter(); // Called when A12 changes from 0 to 1

	CPU2A03* m_cpu;
	bool m_batteryBackedRAM;

	MirroringMode m_mirroringMode;
	size_t m_numPRGROMBanks;
	BankSelectRegister m_bankSelectRegister;
	std::array<uint8_t, MAPPER_004_NUM_REGISTERS> m_registers;
	
	bool m_A12State; // 0 - reading BG info (pattern table 0). 1 - Reading sprite info (pattern table 1)
	uint8_t m_irqCounter;
	uint8_t m_irqLatchValue;
	bool m_irqReload;
	bool m_irqEnabled;
};
