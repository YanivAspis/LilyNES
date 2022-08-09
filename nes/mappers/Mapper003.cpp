#include "Mapper003.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Mapper003::Mapper003(const INESFile& romFile) : Cartridge(romFile)
{
	m_numPRGROMBanks = romFile.GetHeader().GetNumPRGRomBanks();
	m_numCHRROMBanks = romFile.GetHeader().GetNumCHRRomBanks();
	m_mirroringMode = (romFile.GetHeader().GetFixedMirroringMode() == FIXED_MIRROR_HORIZONTAL) ? MIRRORING_HORIZONTAL : MIRRORING_VERTICAL;
	this->SetupLogicalBanks();
	this->InitializeBankMapping();
}

void Mapper003::SetupLogicalBanks()
{
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_003_PRG_FIRST_BANK_START, MAPPER_003_PRG_BANK_SIZE));
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_003_PRG_FIRST_BANK_START + MAPPER_003_PRG_BANK_SIZE, MAPPER_003_PRG_BANK_SIZE));
	m_CHRLogicalBanks.push_back(LogicalBank(MAPPER_003_CHR_FIRST_BANK_START, MAPPER_003_CHR_BANK_SIZE));
}

void Mapper003::InitializeBankMapping()
{
	m_PRGBankMapping[0] = 0;
	m_PRGBankMapping[1] = (m_numPRGROMBanks > 1) ? MAPPER_003_PRG_BANK_SIZE : 0;
	m_CHRBankMapping[0] = 0;
}

MirroringMode Mapper003::GetCurrentMirroringMode()
{
	return m_mirroringMode;
}

std::any Mapper003::GetAdditionalState() const { return 0; }

void Mapper003::LoadAdditionalState(std::any state) {}

void Mapper003::PRGROMWrite(uint16_t address, uint8_t data)
{
	// I am ignoring bus conflicts, since not all boards have those, and games would work around them in software anyways
	uint8_t bankIndex;
	if (m_numCHRROMBanks <= MAPPER_003_STANDARD_NUM_BANKS) {
		bankIndex = ClearUpperBits8(data, MAPPER_003_BANK_SELECT_STANDARD_HIGH_BIT + 1);
	}
	else {
		bankIndex = data;
	}
	m_CHRBankMapping[0] = bankIndex * MAPPER_003_CHR_BANK_SIZE;
}