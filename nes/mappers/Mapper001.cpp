#include <assert.h>

#include "Mapper001.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Mapper001State::Mapper001State() {
	loadData = 0;
	loadCounter = MAPPER_001_LOAD_REGISTER_NUM_WRITES;
	controlRegister.flags.mirroring = MAPPER_001_MIRRORING_SINGLE_SCREEN_LOW;
	controlRegister.flags.PRGBankMode = MAPPER_001_PRG_BANK_16K_HIGH_FIXED;
	controlRegister.flags.CHRBankMode = MAPPER_001_CHR_BANK_8K;
	controlRegister.flags.unused = 0;
}

Mapper001::Mapper001(const INESFile& romFile) : Cartridge(romFile)
{
	m_numPRGROMBanks = romFile.GetHeader().GetNumPRGRomBanks();
	m_batteryBackedRAM = romFile.GetHeader().IsPRGRAMBatteryBacked();
	m_controlRegister.flags.mirroring = MAPPER_001_MIRRORING_SINGLE_SCREEN_LOW;
	m_controlRegister.flags.PRGBankMode = MAPPER_001_PRG_BANK_16K_HIGH_FIXED;
	m_controlRegister.flags.CHRBankMode = MAPPER_001_CHR_BANK_8K;
	m_controlRegister.flags.unused = 0;
	m_loadCounter = MAPPER_001_LOAD_REGISTER_NUM_WRITES;
	this->SetupLogicalBanks();
	this->InitializeBankMapping();
}

void Mapper001::SoftReset()
{
	Cartridge::SoftReset();
	this->ResetLoadRegister();
	m_controlRegister.flags.mirroring = MAPPER_001_MIRRORING_SINGLE_SCREEN_LOW;
	m_controlRegister.flags.PRGBankMode = MAPPER_001_PRG_BANK_16K_HIGH_FIXED;
	m_controlRegister.flags.CHRBankMode = MAPPER_001_CHR_BANK_8K;
	m_controlRegister.flags.unused = 0;
}

void Mapper001::HardReset()
{
	Cartridge::HardReset();
	this->ResetLoadRegister();
	m_controlRegister.flags.mirroring = MAPPER_001_MIRRORING_SINGLE_SCREEN_LOW;
	m_controlRegister.flags.PRGBankMode = MAPPER_001_PRG_BANK_16K_HIGH_FIXED;
	m_controlRegister.flags.CHRBankMode = MAPPER_001_CHR_BANK_8K;
	m_controlRegister.flags.unused = 0;
}

void Mapper001::SetupLogicalBanks()
{
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_001_PRG_FIRST_BANK_START, MAPPER_001_PRG_BANK_SIZE));
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_001_PRG_FIRST_BANK_START + MAPPER_001_PRG_BANK_SIZE, MAPPER_001_PRG_BANK_SIZE));
	m_CHRLogicalBanks.push_back(LogicalBank(MAPPER_001_CHR_FIRST_BANK_START, MAPPER_001_CHR_BANK_SIZE));
	m_CHRLogicalBanks.push_back(LogicalBank(MAPPER_001_CHR_FIRST_BANK_START + MAPPER_001_CHR_BANK_SIZE, MAPPER_001_CHR_BANK_SIZE));
}

void Mapper001::InitializeBankMapping()
{
	m_PRGBankMapping[0] = 0;
	m_PRGBankMapping[1] = (m_numPRGROMBanks - 1) * MAPPER_001_PRG_BANK_SIZE;
	m_CHRBankMapping[0] = 0;
	m_CHRBankMapping[1] = MAPPER_001_CHR_BANK_SIZE;
}

MirroringMode Mapper001::GetCurrentMirroringMode()
{
	// Dynamically chooses mirroring mode
	switch (m_controlRegister.flags.mirroring) {
	case MAPPER_001_MIRRORING_SINGLE_SCREEN_LOW:
	case MAPPER_001_MIRRORING_SINGLE_SCREEN_HIGH:
		return MIRRORING_SINGLE_SCREEN;
	case MAPPER_001_MIRRORING_VERTICAL:
		return MIRRORING_VERTICAL;
	case MAPPER_001_MIRRORING_HORIZONTAL:
		return MIRRORING_HORIZONTAL;
	}

	// Should not reach here
	assert(false);
	return MIRRORING_SINGLE_SCREEN;
}

std::any Mapper001::GetAdditionalState() const
{
	Mapper001State state;
	state.loadData = m_loadData;
	state.loadCounter = m_loadCounter;
	state.controlRegister.value = m_controlRegister.value;
	return state;
}

void Mapper001::LoadAdditionalState(std::any state)
{
	Mapper001State mapperState = std::any_cast<Mapper001State>(state);
	m_loadData = mapperState.loadData;
	m_loadCounter = mapperState.loadCounter;
	m_controlRegister.value = mapperState.controlRegister.value;
}

void Mapper001::PRGROMWrite(uint16_t address, uint8_t data)
{
	if (TestBit8(data, MAPPER_001_LOAD_REGISTER_RESET_BIT)) {
		this->ResetLoadRegister();
		// For some reason, explicitly resetting the load register also sets the PRG Bank select mode to 3
		m_controlRegister.flags.PRGBankMode = MAPPER_001_PRG_BANK_16K_HIGH_FIXED;
		return;
	}

	if (TestBit8(data, MAPPER_001_LOAD_REGISTER_DATA_BIT)) {
		SetBit8(m_loadData, 7);
	}
	m_loadData = ShiftRight8(m_loadData, 1);
	m_loadCounter--;

	if (m_loadCounter == 0) {
		m_loadData = ShiftRight8(m_loadData, 2);
		this->RegisterWrite(address);
		this->ResetLoadRegister();
	}
}

void Mapper001::RegisterWrite(uint16_t address) {
	uint8_t regSelect = TestBit16(address, MAPPER_001_REGISTER_SELECT_LOW_BIT) | ShiftLeft8(TestBit16(address, MAPPER_001_REGISTER_SELECT_HIGH_BIT), 1);
	switch (regSelect) {
	case MAPPER_001_CONTROL_SELECT:
		this->ControlRegisterWrite();
		break;
	case MAPPER_001_CHR_BANK_0_SELECT:
		this->CHRBank0Write();
		break;
	case MAPPER_001_CHR_BANK_1_SELECT:
		this->CHRBank1Write();
		break;
	case MAPPER_001_PRG_BANK_SELECT:
		this->PRGBankWrite();
		break;
	}
}

void Mapper001::ControlRegisterWrite() {
	m_controlRegister.value = m_loadData;
}

void Mapper001::CHRBank0Write() {
	if (m_controlRegister.flags.CHRBankMode == MAPPER_001_CHR_BANK_4K) {
		m_CHRBankMapping[0] = m_loadData * MAPPER_001_CHR_BANK_SIZE;
	}
	else {
		// Low bit ignored in 8 bit mode - but banks are twice as large
		ClearBit8(m_loadData, 0);
		m_CHRBankMapping[0] = m_loadData * MAPPER_001_CHR_BANK_SIZE;
		m_CHRBankMapping[1] = m_CHRBankMapping[0] + MAPPER_001_CHR_BANK_SIZE;
	}
}

void Mapper001::CHRBank1Write() {
	if (m_controlRegister.flags.CHRBankMode == MAPPER_001_CHR_BANK_4K) {
		m_CHRBankMapping[1] = m_loadData * MAPPER_001_CHR_BANK_SIZE;
	}
}

void Mapper001::PRGBankWrite() {
	// Bit 4 ignored in my implementation
	ClearBit8(m_loadData, 4);

	switch (m_controlRegister.flags.PRGBankMode) {
	case MAPPER_001_PRG_BANK_32K_0:
	case MAPPER_001_PRG_BANK_32K_1:
		// Low bit ignored in 8 bit mode - but banks are twice as large
		ClearBit8(m_loadData, 0);
		m_PRGBankMapping[0] = m_loadData * MAPPER_001_PRG_BANK_SIZE;
		m_PRGBankMapping[1] = m_PRGBankMapping[0] + MAPPER_001_PRG_BANK_SIZE;
		break;
	case MAPPER_001_PRG_BANK_16K_LOW_FIXED:
		// Low 16K are fixed to first bank, High 16K are selected
		m_PRGBankMapping[0] = 0;
		m_PRGBankMapping[1] = m_loadData * MAPPER_001_PRG_BANK_SIZE;
		break;
	case MAPPER_001_PRG_BANK_16K_HIGH_FIXED:
		// Low 16K are selected, High 16K are fixed to last bank
		m_PRGBankMapping[0] = m_loadData * MAPPER_001_PRG_BANK_SIZE;
		m_PRGBankMapping[1] = (m_numPRGROMBanks - 1) * MAPPER_001_PRG_BANK_SIZE;
		break;
	}
}

void Mapper001::ResetLoadRegister() {
	m_loadData = 0;
	m_loadCounter = MAPPER_001_LOAD_REGISTER_NUM_WRITES;
}