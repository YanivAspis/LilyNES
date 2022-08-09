#include "Mapper002.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Mapper002::Mapper002(const INESFile& romFile) : Cartridge(romFile)
{
	m_numPRGROMBanks = romFile.GetHeader().GetNumPRGRomBanks();
	m_mirroringMode = (romFile.GetHeader().GetFixedMirroringMode() == FIXED_MIRROR_HORIZONTAL) ? MIRRORING_HORIZONTAL : MIRRORING_VERTICAL;
	this->SetupLogicalBanks();
	this->InitializeBankMapping();
}

void Mapper002::SetupLogicalBanks()
{
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_002_PRG_FIRST_BANK_START, MAPPER_002_PRG_BANK_SIZE));
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_002_PRG_FIRST_BANK_START + MAPPER_002_PRG_BANK_SIZE, MAPPER_002_PRG_BANK_SIZE));
	m_CHRLogicalBanks.push_back(LogicalBank(MAPPER_002_CHR_FIRST_BANK_START, MAPPER_002_CHR_BANK_SIZE));
}

void Mapper002::InitializeBankMapping()
{
	m_PRGBankMapping[0] = 0;
	m_PRGBankMapping[1] = (m_numPRGROMBanks - 1) * MAPPER_002_PRG_BANK_SIZE;
	m_CHRBankMapping[0] = 0;
}

MirroringMode Mapper002::GetCurrentMirroringMode()
{
	return m_mirroringMode;
}

std::any Mapper002::GetAdditionalState() const { return 0; }

void Mapper002::LoadAdditionalState(std::any state) {}

void Mapper002::PRGROMWrite(uint16_t address, uint8_t data)
{
	// I am ignoring bus conflicts, since not all boards have those, and games would work around them in software anyways
	uint8_t bankIndex;
	if (m_numPRGROMBanks <= MAPPER_002_STANDARD_NUM_BANKS) {
		bankIndex = ClearUpperBits8(data, MAPPER_002_BANK_SELECT_STANDARD_HIGH_BIT+1);
	}
	else {
		bankIndex = ClearUpperBits8(data, MAPPER_002_BANK_SELECT_NON_STANDARD_HIGH_BIT+1);
	}
	m_PRGBankMapping[0] = bankIndex * MAPPER_002_PRG_BANK_SIZE;
}
