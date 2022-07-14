#include "Cartridge.h"


MapperAdditionalState::~MapperAdditionalState() {}

CartridgeState::CartridgeState() : additionalState(nullptr) {}


Cartridge::Cartridge(const INESFile& romFile) : BusDevice(std::list<AddressRange>({
	AddressRange(PRG_RAM_START_ADDRESS, PRG_RAM_END_ADDRESS), AddressRange(PRG_ROM_START_ADDRESS, PRG_ROM_END_ADDRESS)
	})), m_PRGROM(romFile.GetPRGROM()), m_CHRROM(romFile.GetCHRROM()), m_CHRRAMEnabled(romFile.GetHeader().UsesCHRRam()) 
{
	m_PRGRAM = std::vector<uint8_t>(PRG_RAM_BANK_SIZE * romFile.GetHeader().GetPRGRAMSize(), 0);
}


// Soft reset does not change PRG RAM content?
void Cartridge::SoftReset() {}


// Perhaps should be set to random values?
void Cartridge::HardReset() {
	m_PRGRAM = std::vector<uint8_t>(m_PRGRAM.size(), 0);
}


CartridgeState Cartridge::GetState() const {
	CartridgeState state;
	state.CHRROM = m_CHRROM;
	state.PRGRAM = m_PRGRAM;
	this->GetAdditionalState(state.additionalState);
	return state;
}

void Cartridge::LoadState(CartridgeState& state) {
	m_CHRROM = state.CHRROM;
	m_PRGRAM = state.PRGRAM;
	this->LoadAdditionalState(state.additionalState);
}