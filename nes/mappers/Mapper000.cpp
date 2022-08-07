#include "Mapper000.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

Mapper000::Mapper000(const INESFile& romFile) 
: Cartridge(romFile)
{
	m_numPRGROMBanks = m_PRGROM.size() / PRG_ROM_BANK_SIZE;
	m_mirroringMode = (romFile.GetHeader().GetFixedMirroringMode() == FIXED_MIRROR_HORIZONTAL) ? MIRRORING_HORIZONTAL : MIRRORING_VERTICAL;
	this->SetupLogicalBanks();
	this->InitializeBankMapping();
}

void Mapper000::SetupLogicalBanks() {
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_000_PRG_FIRST_BANK_START, MAPPER_000_PRG_BANK_SIZE));
	m_PRGLogicalBanks.push_back(LogicalBank(MAPPER_000_PRG_FIRST_BANK_START + MAPPER_000_PRG_BANK_SIZE, MAPPER_000_PRG_BANK_SIZE));
	m_CHRLogicalBanks.push_back(LogicalBank(MAPPER_000_CHR_FIRST_BANK_START, MAPPER_000_CHR_BANK_SIZE));
}

void Mapper000::InitializeBankMapping() {
	m_PRGBankMapping[0] = 0;
	m_PRGBankMapping[1] = (m_numPRGROMBanks > 1) ? MAPPER_000_PRG_BANK_SIZE : 0;
	m_CHRBankMapping[0] = 0;
}

MirroringMode Mapper000::GetCurrentMirroringMode() {
	return m_mirroringMode;
}


void Mapper000::GetAdditionalState(std::shared_ptr<MapperAdditionalState> state) const {}

void Mapper000::LoadAdditionalState(std::shared_ptr<MapperAdditionalState> state) {}

void Mapper000::PRGROMWrite(uint16_t address, uint8_t data) {
	// Can't write to ROM, ingore
	return;
}