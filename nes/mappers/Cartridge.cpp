#include <assert.h>
#include "Cartridge.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

LogicalBank::LogicalBank(uint16_t sAddress, uint16_t bankSize) {
	assert(sAddress + bankSize <= (PRG_ROM_END_ADDRESS+1));
	startAddress = sAddress;
	size = bankSize;
}

MapperAdditionalState::~MapperAdditionalState() {}

CartridgeState::CartridgeState() {
	mapperID = 0;
}


Cartridge::Cartridge(const INESFile& romFile) : BusDevice(std::list<AddressRange>({
	AddressRange(PRG_RAM_START_ADDRESS, PRG_RAM_END_ADDRESS), AddressRange(PRG_ROM_START_ADDRESS, PRG_ROM_END_ADDRESS)
	})), m_PRGROM(romFile.GetPRGROM()), m_CHRROM(romFile.GetCHRROM()), m_CHRRAMEnabled(romFile.GetHeader().UsesCHRRam()) 
{
	m_PRGRAM = std::vector<uint8_t>(PRG_RAM_BANK_SIZE * romFile.GetHeader().GetPRGRAMSize(), 0);
	if (m_CHRRAMEnabled) {
		m_CHRROM.resize(CHR_RAM_BANK_SIZE);
	}

	m_mapperID = romFile.GetHeader().GetMapperId();
}


// Soft reset does not change PRG RAM content?
void Cartridge::SoftReset() {
	this->InitializeBankMapping();
}


void Cartridge::HardReset() {
	// Perhaps should be set to random values?
	m_PRGRAM = std::vector<uint8_t>(m_PRGRAM.size(), 0);

	this->InitializeBankMapping();
}

// Standard PRGROM mapping
void Cartridge::Read(uint16_t address, uint8_t& data) {
	if (address >= PRG_RAM_START_ADDRESS && address <= PRG_RAM_END_ADDRESS) {
		data = this->PRGRAMRead(address);
	}
	else if (address >= PRG_ROM_START_ADDRESS && address <= PRG_ROM_END_ADDRESS) {
		data = this->PRGROMRead(address);
	}

	// Not supposed to reach here
	assert(false);
}

void Cartridge::Write(uint16_t address, uint8_t data) {
	if (address >= PRG_RAM_START_ADDRESS && address <= PRG_RAM_END_ADDRESS) {
		this->PRGRAMWrite(address, data);
	}
	else if (address >= PRG_ROM_START_ADDRESS && address <= PRG_ROM_END_ADDRESS) {
		this->PRGROMWrite(address, data);
	}
}

// Standard CHRROM mapping (overridable by mapper)
uint8_t Cartridge::PPURead(uint16_t address) {
	// Mirror to Pattern Table range
	address = ClearUpperBits16(address, 13);
	return this->CHRROMRead(address);
	
}

// Standard CHRRAM mapping (overridable by mapper)
void Cartridge::PPUWrite(uint16_t address, uint8_t data) {
	if (m_CHRRAMEnabled) {
		// Mirror to Pattern Table range
		address = ClearUpperBits16(address, 13);
		this->CHRROMWrite(address, data);
	}
}

uint8_t Cartridge::Probe(uint16_t address) {
	uint8_t data = 0;
	this->Read(address, data);
	return data;
}

uint8_t Cartridge::ProbePPU(uint16_t address) {
	return this->PPURead(address);
}


CartridgeState Cartridge::GetState() const {
	CartridgeState state;
	state.mapperID = m_mapperID;
	state.CHRROM = m_CHRROM;
	state.PRGRAM = m_PRGRAM;
	state.PRGLogicalBanks = m_PRGLogicalBanks;
	state.CHRLogicalBanks = m_CHRLogicalBanks;
	state.PRGBankMapping = m_PRGBankMapping;
	state.CHRBankMapping = m_CHRBankMapping;
	state.additionalState = this->GetAdditionalState();
	return state;
}

void Cartridge::LoadState(CartridgeState& state) {
	m_CHRROM = state.CHRROM;
	m_PRGRAM = state.PRGRAM;
	m_PRGLogicalBanks = state.PRGLogicalBanks;
	m_CHRLogicalBanks = state.CHRLogicalBanks;
	m_PRGBankMapping = state.PRGBankMapping;
	m_CHRBankMapping = state.CHRBankMapping;
	this->LoadAdditionalState(state.additionalState);
}

// Standard mapping operations (overridable by mappers)

uint8_t Cartridge::PRGROMRead(uint16_t address) {
	unsigned int bank = this->GetLogicalBankIndex(m_PRGLogicalBanks, address);
	size_t index = m_PRGBankMapping[bank] + (size_t)(address - m_PRGLogicalBanks[bank].startAddress);
	return m_PRGROM[index];
}

uint8_t Cartridge::PRGRAMRead(uint16_t address) {
	address -= PRG_RAM_START_ADDRESS;

	// If address out of range, return 0? 
	// Or perhaps open bus behaviour should be implemented?
	if (address < (uint16_t)m_PRGRAM.size()) {
		return m_PRGRAM[address];
	}
	else {
		return 0;
	}
}

void Cartridge::PRGRAMWrite(uint16_t address, uint8_t data) {
	address -= PRG_RAM_START_ADDRESS;

	// If address out of range, ignore
	if (address < (uint16_t)m_PRGRAM.size()) {
		m_PRGRAM[address] = data;
	}
}

uint8_t Cartridge::CHRROMRead(uint16_t address) {
	unsigned int bank = this->GetLogicalBankIndex(m_CHRLogicalBanks, address);
	size_t index = m_CHRBankMapping[bank] + (size_t)(address - m_CHRLogicalBanks[bank].startAddress);
	return m_CHRROM[index];
}

void Cartridge::CHRROMWrite(uint16_t address, uint8_t data) {
	unsigned int bank = this->GetLogicalBankIndex(m_CHRLogicalBanks, address);
	size_t index = m_CHRBankMapping[bank] + (size_t)(address - m_CHRLogicalBanks[bank].startAddress);
	m_CHRROM[index] = data;
}

unsigned int Cartridge::GetLogicalBankIndex(std::vector<LogicalBank>& logicalBanks, uint16_t address) {
	for (unsigned int i = 0; i < logicalBanks.size(); i++) {
		if (address >= logicalBanks[i].startAddress && address < (logicalBanks[i].startAddress + logicalBanks[i].size)) {
			return i;
		}
	}
	// Should not be reached
	assert(false);
	return 0;
}