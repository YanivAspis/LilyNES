#include <assert.h>

#include "Mapper004.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Mapper004State::Mapper004State() {
	mirroringMode = MIRRORING_VERTICAL;
	bankSelectRegister.value = 0;
	registers.fill(0);

	A12State = false;
	irqCounter = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	irqLatchValue = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	irqReload = false;
	irqEnabled = false;
}

Mapper004::Mapper004(const INESFile& romFile, CPU2A03* cpu) : Cartridge(romFile)
{
	m_cpu = cpu;
	m_numPRGROMBanks = 2 * romFile.GetHeader().GetNumPRGRomBanks();
	m_batteryBackedRAM = romFile.GetHeader().IsPRGRAMBatteryBacked();

	m_mirroringMode = MIRRORING_VERTICAL;
	m_bankSelectRegister.value = 0;
	m_registers.fill(0);
	
	m_A12State = false;
	m_irqCounter = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqLatchValue = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqReload = false;
	m_irqEnabled = false;
}

Mapper004::~Mapper004()
{
	m_cpu = nullptr;
}

void Mapper004::SoftReset() {
	m_mirroringMode = MIRRORING_VERTICAL;
	m_bankSelectRegister.value = 0;
	m_registers.fill(0);

	m_A12State = false;
	m_irqCounter = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqLatchValue = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqReload = false;
	m_irqEnabled = false;
}

void Mapper004::HardReset() {
	m_mirroringMode = MIRRORING_VERTICAL;
	m_bankSelectRegister.value = 0;
	m_registers.fill(0);

	m_A12State = false;
	m_irqCounter = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqLatchValue = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqReload = false;
	m_irqEnabled = false;
}

// Not using logical banks for this mapper
void Mapper004::SetupLogicalBanks() {}
void Mapper004::InitializeBankMapping() {}

MirroringMode Mapper004::GetCurrentMirroringMode()
{
	return m_mirroringMode;
}


uint8_t Mapper004::ProbePPU(uint16_t address) {
	if (m_bankSelectRegister.flags.CHRROMBankMode == 0) {
		return this->CHRROMReadMode0(address);
	}
	else {
		return this->CHRROMReadMode1(address);
	}
}

std::any Mapper004::GetAdditionalState() const
{
	Mapper004State state;
	state.mirroringMode = m_mirroringMode;
	state.bankSelectRegister.value = m_bankSelectRegister.value;
	state.registers = m_registers;

	state.A12State = m_A12State;
	state.irqCounter = m_irqCounter;
	state.irqLatchValue = m_irqLatchValue;
	state.irqReload = m_irqReload;
	state.irqEnabled = m_irqEnabled;

	return state;
}

void Mapper004::LoadAdditionalState(std::any state)
{
	Mapper004State mapperState = std::any_cast<Mapper004State>(state);

	m_mirroringMode = mapperState.mirroringMode;
	m_bankSelectRegister.value = mapperState.bankSelectRegister.value;
	m_registers = mapperState.registers;

	m_A12State = mapperState.A12State;
	m_irqCounter = mapperState.irqCounter;
	m_irqLatchValue = mapperState.irqLatchValue;
	m_irqReload = mapperState.irqReload;
	m_irqEnabled = mapperState.irqEnabled;
}

void Mapper004::PRGROMWrite(uint16_t address, uint8_t data)
{
	if (address >= MAPPER_004_BANK_SELECT_WRITE_START_ADDRESS && address <= MAPPER_004_BANK_SELECT_WRITE_END_ADDRESS) {
		if (address % 2 == 0) {
			this->BankSelectWrite(data);
		}
		else {
			this->BankDataWrite(data);
		}
	}
	else if (address >= MAPPER_004_MIRRORING_PRGRAM_WRITE_START_ADDRESS && address <= MAPPER_004_MIRRORING_PRGRAM_WRITE_END_ADDRESS) {
		if (address % 2 == 0) {
			this->MirroringWrite(data);
		}
		else {
			this->PRGRAMProtectWrite(data);
		}
	}
	else if (address >= MAPPER_004_IRQ_LATCH_WRITE_START_ADDRESS && address <= MAPPER_004_IRQ_LATCH_WRITE_END_ADDRESS) {
		if (address % 2 == 0) {
			this->IRQLatchWrite(data);
		}
		else {
			this->IRQReloadWrite();
		}
	}
	else if (address >= MAPPER_004_IRQ_ENABLE_WRITE_START_ADDRESS && address <= MAPPER_004_IRQ_ENABLE_WRITE_END_ADDRESS) {
		if (address % 2 == 0) {
			this->IRQDisableWrite();
		}
		else {
			this->IRQEnableWrite();
		}
	}
}

void Mapper004::BankSelectWrite(uint8_t data) {
	m_bankSelectRegister.value = data;
}

void Mapper004::BankDataWrite(uint8_t data) {
	if (m_bankSelectRegister.flags.registerSelect >= 0 && m_bankSelectRegister.flags.registerSelect <= 1) {
		// Low bit is ignored for 2KB CHRROM registers
		ClearBit8(data, 0);
	}
	else if (m_bankSelectRegister.flags.registerSelect >= 6 && m_bankSelectRegister.flags.registerSelect <= 7) {
		// PRG Banks ignore top two bits
		ClearBit8(data, 6);
		ClearBit8(data, 7);

		// TODO: Correct bank mirroring?
		data %= m_numPRGROMBanks;
	}
	m_registers[m_bankSelectRegister.flags.registerSelect] = data;
}

void Mapper004::MirroringWrite(uint8_t data) {
	m_mirroringMode = TestBit8(data, 0) ? MIRRORING_HORIZONTAL : MIRRORING_VERTICAL;
}

// Not implementing PRGRAM protect for compatibility with MMC6
void Mapper004::PRGRAMProtectWrite(uint8_t data) {}

void Mapper004::IRQLatchWrite(uint8_t data) {
	m_irqLatchValue = data;
}

void Mapper004::IRQReloadWrite() {
	m_irqReload = true;
}

void Mapper004::IRQDisableWrite() {
	m_irqEnabled = false;
}

void Mapper004::IRQEnableWrite() {
	m_irqEnabled = true;
}

uint8_t Mapper004::PRGROMRead(uint16_t address)
{
	if (m_bankSelectRegister.flags.PRGROMBankMode == 0) {
		return this->PRGROMReadMode0(address);
	}
	else {
		return this->PRGROMReadMode1(address);
	}
}

uint8_t Mapper004::PRGROMReadMode0(uint16_t address)
{
	if (address >= MAPPER_004_PRGROM_MODE0_REGISTER_6_START && address <= MAPPER_004_PRGROM_MODE0_REGISTER_6_END) {
		return m_PRGROM[m_registers[6] * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE0_REGISTER_6_START];
	} 
	else if (address >= MAPPER_004_PRGROM_MODE0_REGISTER_7_START && address <= MAPPER_004_PRGROM_MODE0_REGISTER_7_END) {
		return m_PRGROM[m_registers[7] * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE0_REGISTER_7_START];
	}
	else if (address >= MAPPER_004_PRGROM_MODE0_LOW_FIXED_START && address <= MAPPER_004_PRGROM_MODE0_LOW_FIXED_END) {
		return m_PRGROM[(m_numPRGROMBanks - 2) * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE0_LOW_FIXED_START];
	}
	else if (address >= MAPPER_004_PRGROM_MODE0_HIGH_FIXED_START && address <= MAPPER_004_PRGROM_MODE0_HIGH_FIXED_END) {
		return m_PRGROM[(m_numPRGROMBanks - 1) * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE0_HIGH_FIXED_START];
	}

	// Should not reach here
	assert(false);
	return 0;
}

uint8_t Mapper004::PRGROMReadMode1(uint16_t address)
{
	if (address >= MAPPER_004_PRGROM_MODE1_LOW_FIXED_START && address <= MAPPER_004_PRGROM_MODE1_LOW_FIXED_END) {
		return m_PRGROM[(m_numPRGROMBanks - 2) * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE1_LOW_FIXED_START];
	}
	else if (address >= MAPPER_004_PRGROM_MODE1_REGISTER_7_START && address <= MAPPER_004_PRGROM_MODE1_REGISTER_7_END) {
		return m_PRGROM[m_registers[7] * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE1_REGISTER_7_START];
	}
	else if (address >= MAPPER_004_PRGROM_MODE1_REGISTER_6_START && address <= MAPPER_004_PRGROM_MODE1_REGISTER_6_END) {
		return m_PRGROM[m_registers[6] * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE1_REGISTER_6_START];
	}
	else if (address >= MAPPER_004_PRGROM_MODE1_HIGH_FIXED_START && address <= MAPPER_004_PRGROM_MODE1_HIGH_FIXED_END) {
		return m_PRGROM[(m_numPRGROMBanks - 1) * MAPPER_004_PRG_BANK_SIZE + address - MAPPER_004_PRGROM_MODE1_HIGH_FIXED_START];
	}

	// Should not reach here
	assert(false);
	return 0;
}

uint8_t Mapper004::CHRROMRead(uint16_t address)
{
	if (TestBit16(address, MAPPER_004_PATTERN_TABLE_TEST_BIT) == 0) {
		m_A12State = false;
	}
	else {
		if (!m_A12State) {
			this->DecrementIRQCounter();
		}
		m_A12State = true;
	}

	if (m_bankSelectRegister.flags.CHRROMBankMode == 0) {
		return this->CHRROMReadMode0(address);
	}
	else {
		return this->CHRROMReadMode1(address);
	}
}

uint8_t Mapper004::CHRROMReadMode0(uint16_t address)
{
	if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_0_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_0_END) {
		return m_CHRROM[m_registers[0] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_0_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_1_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_1_END) {
		return m_CHRROM[m_registers[1] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_1_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_2_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_2_END) {
		return m_CHRROM[m_registers[2] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_2_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_3_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_3_END) {
		return m_CHRROM[m_registers[3] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_3_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_4_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_4_END) {
		return m_CHRROM[m_registers[4] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_4_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_5_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_5_END) {
		return m_CHRROM[m_registers[5] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_5_START];
	}

	// Should not reach here
	assert(false);
	return 0;
}

uint8_t Mapper004::CHRROMReadMode1(uint16_t address)
{
	if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_2_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_2_END) {
		return m_CHRROM[m_registers[2] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_2_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_3_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_3_END) {
		return m_CHRROM[m_registers[3] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_3_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_4_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_4_END) {
		return m_CHRROM[m_registers[4] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_4_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_5_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_5_END) {
		return m_CHRROM[m_registers[5] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_5_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_0_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_0_END) {
		return m_CHRROM[m_registers[0] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_0_START];
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_1_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_1_END) {
		return m_CHRROM[m_registers[1] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_1_START];
	}

	// Should not reach here
	assert(false);
	return 0;
}

void Mapper004::CHRROMWrite(uint16_t address, uint8_t data) {
	if (TestBit16(address, MAPPER_004_PATTERN_TABLE_TEST_BIT) == 0) {
		m_A12State = false;
	}
	else {
		if (!m_A12State) {
			this->DecrementIRQCounter();
		}
		m_A12State = true;
	}

	if (m_bankSelectRegister.flags.CHRROMBankMode == 0) {
		this->CHRROMWriteMode0(address, data);
	}
	else {
		this->CHRROMWriteMode1(address , data);
	}
}

void Mapper004::CHRROMWriteMode0(uint16_t address, uint8_t data) {
	if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_0_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_0_END) {
		m_CHRROM[m_registers[0] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_0_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_1_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_1_END) {
		m_CHRROM[m_registers[1] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_1_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_2_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_2_END) {
		m_CHRROM[m_registers[2] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_2_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_3_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_3_END) {
		m_CHRROM[m_registers[3] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_3_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_4_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_4_END) {
		m_CHRROM[m_registers[4] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_4_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE0_REGISTER_5_START && address <= MAPPER_004_CHRROM_MODE0_REGISTER_5_END) {
		m_CHRROM[m_registers[5] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE0_REGISTER_5_START] = data;
	}
}

void Mapper004::CHRROMWriteMode1(uint16_t address, uint8_t data) {
	if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_2_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_2_END) {
		m_CHRROM[m_registers[2] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_2_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_3_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_3_END) {
		m_CHRROM[m_registers[3] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_3_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_4_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_4_END) {
		m_CHRROM[m_registers[4] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_4_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_5_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_5_END) {
		m_CHRROM[m_registers[5] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_5_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_0_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_0_END) {
		m_CHRROM[m_registers[0] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_0_START] = data;
	}
	else if (address >= MAPPER_004_CHRROM_MODE1_REGISTER_1_START && address <= MAPPER_004_CHRROM_MODE1_REGISTER_1_END) {
		m_CHRROM[m_registers[1] * MAPPER_004_CHR_BANK_SIZE + address - MAPPER_004_CHRROM_MODE1_REGISTER_1_START] = data;
	}
}


void Mapper004::DecrementIRQCounter()
{
	// "Old" or "alternate" IRQ behaviour seems to be what most games expect
	if (m_irqReload) {
		m_irqReload = false;
		m_irqCounter = m_irqLatchValue;
		// When IRQ is reloaded with a value of 0, it fires once
		if (m_irqCounter == 0 && m_irqEnabled) {
			m_cpu->RaiseIRQ();
		}
		// Counter not decremented upon reload
		return;
	}

	// Fire IRQ when decrementing from 1 to 0
	if (m_irqCounter == 1 && m_irqEnabled) {
		m_cpu->RaiseIRQ();
	}

	// Reload IRQ counter only when counter is 0 (so the scanline after firing the IRQ)
	if (m_irqCounter == 0) {
		m_irqCounter = m_irqLatchValue;
	}
	else {
		m_irqCounter--;
	}
}
