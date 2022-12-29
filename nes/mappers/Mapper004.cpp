#include <assert.h>

#include "Mapper004.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Mapper004State::Mapper004State() {
	mirroringMode = MIRRORING_VERTICAL;
	bankSelectRegister.value = 0;

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
	
	m_A12State = false;
	m_irqCounter = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqLatchValue = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqReload = false;
	m_irqEnabled = false;

	this->SetupLogicalBanks();
	this->InitializeBankMapping();
}

Mapper004::~Mapper004()
{
	m_cpu = nullptr;
}

void Mapper004::SoftReset() {
	Cartridge::SoftReset();
	m_mirroringMode = MIRRORING_VERTICAL;
	m_bankSelectRegister.value = 0;

	m_A12State = false;
	m_irqCounter = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqLatchValue = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqReload = false;
	m_irqEnabled = false;
}

void Mapper004::HardReset() {
	Cartridge::HardReset();
	m_mirroringMode = MIRRORING_VERTICAL;
	m_bankSelectRegister.value = 0;

	m_A12State = false;
	m_irqCounter = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqLatchValue = MAPPER_004_IRQ_COUNTER_INITIAL_VALUE;
	m_irqReload = false;
	m_irqEnabled = false;
}

// Not using logical banks for this mapper
void Mapper004::SetupLogicalBanks() {
	for (unsigned int i = 0; i < MAPPER_004_NUM_PRG_LOGICAL_BANKS; i++) {
		m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_004_PRG_FIRST_BANK_START + i * MAPPER_004_PRG_BANK_SIZE, MAPPER_004_PRG_BANK_SIZE));
	}

	for (unsigned int i = 0; i < MAPPER_004_NUM_CHR_LOGICAL_BANKS; i++) {
		m_CHRLogicalBanks.push_back(LogicalBank(MAPPER_004_CHR_FIRST_BANK_START + i * MAPPER_004_CHR_BANK_SIZE, MAPPER_004_CHR_BANK_SIZE));
	}
}
void Mapper004::InitializeBankMapping() {
	m_PRGBankMapping[0] = 0;
	m_PRGBankMapping[1] = 0;
	m_PRGBankMapping[2] = (m_numPRGROMBanks - 2) * MAPPER_004_PRG_BANK_SIZE;
	m_PRGBankMapping[3] = (m_numPRGROMBanks - 1) * MAPPER_004_PRG_BANK_SIZE;

	for (unsigned int i = 0; i < MAPPER_004_NUM_CHR_LOGICAL_BANKS; i++) {
		m_CHRBankMapping[i] = 0;
	}
}

MirroringMode Mapper004::GetCurrentMirroringMode()
{
	return m_mirroringMode;
}


uint8_t Mapper004::ProbePPU(uint16_t address) {
	address = ClearUpperBits16(address, 13);
	return Cartridge::CHRROMRead(address);
}

std::vector<uint8_t> Mapper004::GetAdditionalState() const
{
	Mapper004State state;
	state.mirroringMode = m_mirroringMode;
	state.bankSelectRegister.value = m_bankSelectRegister.value;

	state.A12State = m_A12State;
	state.irqCounter = m_irqCounter;
	state.irqLatchValue = m_irqLatchValue;
	state.irqReload = m_irqReload;
	state.irqEnabled = m_irqEnabled;

	std::vector<uint8_t> stateVector;
	stateVector.resize(sizeof(state));
	std::memcpy(stateVector.data(), &state, sizeof(state));

	return stateVector;
}

void Mapper004::LoadAdditionalState(const std::vector<uint8_t>& state)
{
	Mapper004State mapperState;
	std::memcpy(&mapperState, state.data(), sizeof(mapperState));

	m_mirroringMode = mapperState.mirroringMode;
	m_bankSelectRegister.value = mapperState.bankSelectRegister.value;

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
	BankSelectRegister newRegister;
	newRegister.value = data;
	if (newRegister.flags.PRGROMBankMode != m_bankSelectRegister.flags.PRGROMBankMode) {
		// Switch PRG Bank mode
		std::map<unsigned int, size_t> oldMapping = m_PRGBankMapping;
		m_PRGBankMapping[0] = oldMapping[2];
		m_PRGBankMapping[2] = oldMapping[0];
	}
	if (newRegister.flags.CHRROMBankMode != m_bankSelectRegister.flags.CHRROMBankMode) {
		// Switch CHR Bank mode
		std::map<unsigned int, size_t> oldMapping = m_CHRBankMapping;
		m_CHRBankMapping[0] = oldMapping[4];
		m_CHRBankMapping[1] = oldMapping[5];
		m_CHRBankMapping[2] = oldMapping[6];
		m_CHRBankMapping[3] = oldMapping[7];
		m_CHRBankMapping[4] = oldMapping[0];
		m_CHRBankMapping[5] = oldMapping[1];
		m_CHRBankMapping[6] = oldMapping[2];
		m_CHRBankMapping[7] = oldMapping[3];
	}
	m_bankSelectRegister.value = data;
}

void Mapper004::BankDataWrite(uint8_t data) {
	if (m_bankSelectRegister.flags.registerSelect >= 0 && m_bankSelectRegister.flags.registerSelect <= 5) {
		// Switching a CHR Bank
		if (m_bankSelectRegister.flags.registerSelect >= 0 && m_bankSelectRegister.flags.registerSelect <= 1) {
			// Low bit is ignored for 2KB CHRROM registers
			ClearBit8(data, 0);
			m_CHRBankMapping[m_bankSelectRegister.flags.CHRROMBankMode * 4 + m_bankSelectRegister.flags.registerSelect * 2] = data * MAPPER_004_CHR_BANK_SIZE;
			m_CHRBankMapping[m_bankSelectRegister.flags.CHRROMBankMode * 4 + m_bankSelectRegister.flags.registerSelect * 2 + 1] = data * MAPPER_004_CHR_BANK_SIZE + MAPPER_004_CHR_BANK_SIZE;
		}
		else {
			m_CHRBankMapping[(1 - m_bankSelectRegister.flags.CHRROMBankMode) * 4 + m_bankSelectRegister.flags.registerSelect - 2] = data * MAPPER_004_CHR_BANK_SIZE;
		}
	}
	else if (m_bankSelectRegister.flags.registerSelect >= 6 && m_bankSelectRegister.flags.registerSelect <= 7) {
		// PRG Banks ignore top two bits
		ClearBit8(data, 6);
		ClearBit8(data, 7);

		// TODO: Correct bank mirroring?
		data %= m_numPRGROMBanks;
		if (m_bankSelectRegister.flags.registerSelect == 6) {
			m_PRGBankMapping[2 * m_bankSelectRegister.flags.PRGROMBankMode] = data * MAPPER_004_PRG_BANK_SIZE;
		}
		else {
			m_PRGBankMapping[1] = data * MAPPER_004_PRG_BANK_SIZE;
		}
	}
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
	m_cpu->AcknowledgeIRQ(MAPPER_004_IRQ_ID);
	m_irqEnabled = false;
}

void Mapper004::IRQEnableWrite() {
	m_irqEnabled = true;
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

	return Cartridge::CHRROMRead(address);
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

	return Cartridge::CHRROMWrite(address, data);
}

void Mapper004::DecrementIRQCounter()
{
	// "Old" or "alternate" IRQ behaviour seems to be what most games expect
	if (m_irqReload) {
		m_irqReload = false;
		m_irqCounter = m_irqLatchValue;
		// When IRQ is reloaded with a value of 0, it fires once
		if (m_irqCounter == 0 && m_irqEnabled) {
			m_cpu->RaiseIRQ(MAPPER_004_IRQ_ID);
		}
		// Counter not decremented upon reload
		return;
	}

	// Fire IRQ when decrementing from 1 to 0
	if (m_irqCounter == 1 && m_irqEnabled) {
		m_cpu->RaiseIRQ(MAPPER_004_IRQ_ID);
	}

	// Reload IRQ counter only when counter is 0 (so the scanline after firing the IRQ)
	if (m_irqCounter == 0) {
		m_irqCounter = m_irqLatchValue;
	}
	else {
		m_irqCounter--;
	}
}
