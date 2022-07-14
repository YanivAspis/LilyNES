#include "NES.h"
#include "mappers/Mapper000.h"


NES::NES(): m_cpu(false) {
	m_cpuBus.ConnectDevice(&m_RAM);
	m_cpu.ConnectToBus(&m_cpuBus);
	m_cartridge = nullptr;
}

NES::~NES() {
	m_cpuBus.DisconnectAllDevices();
	if (m_cartridge != nullptr) {
		delete m_cartridge;
		m_cartridge = nullptr;
	}
}

void NES::ConnectCartridge(const INESFile& romFile) {
	if (m_cartridge != nullptr) {
		this->DisconnectCartridge();
	}


	switch (romFile.GetHeader().GetMapperId()) {
	case 0x00:
		m_cartridge = new Mapper000(romFile);
		break;
	default:
		throw UnsupportedMapperException(romFile.GetHeader().GetMapperId());
	}

	
	m_cpuBus.ConnectDevice(m_cartridge);
}

void NES::DisconnectCartridge() {
	if (m_cartridge != nullptr) {
		m_cpuBus.DisconnectDevice(m_cartridge);
		delete m_cartridge;
		m_cartridge = nullptr;
	}
}

void NES::SoftReset()
{
	m_cpuBus.SoftReset();
	m_cpu.SoftReset();
}

void NES::HardReset()
{
	m_cpuBus.HardReset();
	m_cpu.HardReset();
}

NESState NES::GetState() const
{
	NESState state;
	state.cpuState = m_cpu.GetState();
	state.ramState = m_RAM.GetState();
	return state;
}

void NES::LoadState(NESState& state)
{
	m_cpu.LoadState(state.cpuState);
	m_RAM.LoadState(state.ramState);
}

void NES::Clock()
{
	m_cpu.Clock();
}
