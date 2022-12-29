#include <assert.h>

#include "NES.h"
#include "mappers/Mapper000.h"
#include "mappers/Mapper001.h"
#include "mappers/Mapper002.h"
#include "mappers/Mapper003.h"
#include "mappers/Mapper004.h"

NESState::NESState() {
	cycleCount = 0;
	DMCDMACycles = 0;
}



NES::NES(Environment* environment) : m_cpu(false), m_ppu(environment, &m_cpu, &m_paletteRAM), m_controllers(environment), 
	m_apu(&m_cpu), m_frameCounterController2(&m_apu, &m_controllers) {

	m_cpuBus.ConnectDevice(&m_RAM);
	m_cpuBus.ConnectDevice(&m_ppu);
	m_cpuBus.ConnectDevice(&m_OAMDMA);
	m_cpuBus.ConnectDevice(&m_controllers);
	m_cpuBus.ConnectDevice(&m_apu);
	m_apu.DMCConnectToBus(&m_cpuBus);
	m_cpuBus.ConnectDevice(&m_frameCounterController2);
	m_ppuBus.ConnectDevice(&m_patternTables);
	m_ppuBus.ConnectDevice(&m_nametables);
	m_ppuBus.ConnectDevice(&m_paletteRAM);
	m_cpu.ConnectToBus(&m_cpuBus);
	m_ppu.ConnectToBus(&m_ppuBus);
	m_cartridge = nullptr;
	m_DMCDMACycles = 0;
	m_cycleCount = 0;
}

NES::~NES() {
	m_ppuBus.DisconnectAllDevices();
	m_apu.DMCDisconnectFromBus();
	m_cpuBus.DisconnectAllDevices();
	if (m_cartridge != nullptr) {
		delete m_cartridge;
		m_cartridge = nullptr;
	}
}

void NES::LoadROM(const INESFile& romFile) {
	if (m_cartridge != nullptr) {
		this->DisconnectCartridge();
	}
	switch (romFile.GetHeader().GetMapperId()) {
	case 0x00:
		m_cartridge = new Mapper000(romFile);
		break;
	case 0x01:
		m_cartridge = new Mapper001(romFile);
		break;
	case 0x02:
		m_cartridge = new Mapper002(romFile);
		break;
	case 0x03:
		m_cartridge = new Mapper003(romFile);
		break;
	case 0x04:
		m_cartridge = new Mapper004(romFile, &m_cpu);
		break;
	default:
		throw UnsupportedMapperException(romFile.GetHeader().GetMapperId());
	}
	this->ConnectCartridge();
}



void NES::SoftReset()
{
	m_cpuBus.SoftReset();
	m_ppuBus.SoftReset();
	m_cpu.SoftReset();
	m_cycleCount = 0;
}

void NES::HardReset()
{
	m_cpuBus.HardReset();
	m_ppuBus.HardReset();
	m_cpu.HardReset();
	m_DMCDMACycles = 0;
	m_cycleCount = 0;
}

NESState NES::GetState() const
{
	NESState state;
	state.cpuState = m_cpu.GetState();
	state.ramState = m_RAM.GetState();
	state.paletteRAMState = m_paletteRAM.GetState();
	state.nametableState = m_nametables.GetState();
	state.ppuState = m_ppu.GetState();
	state.oamDMAState = m_OAMDMA.GetState();
	state.controllerState = m_controllers.GetState();
	state.apuState = m_apu.GetState();
	state.cycleCount = m_cycleCount;
	state.DMCDMACycles = m_DMCDMACycles;
	if (m_cartridge != nullptr) {
		state.patternTableState = m_patternTables.GetState();
		state.cartridgeState = m_cartridge->GetState();
	}
	return state;
}

void NES::LoadState(NESState& state)
{
	m_cpu.LoadState(state.cpuState);
	m_RAM.LoadState(state.ramState);
	m_paletteRAM.LoadState(state.paletteRAMState);
	m_nametables.LoadState(state.nametableState);
	m_ppu.LoadState(state.ppuState);
	m_OAMDMA.LoadState(state.oamDMAState);
	m_controllers.LoadState(state.controllerState);
	m_apu.LoadState(state.apuState);
	m_cycleCount = state.cycleCount;
	m_DMCDMACycles = state.DMCDMACycles;
	if (m_cartridge != nullptr) {
		m_cartridge->LoadState(state.cartridgeState);
		m_patternTables.LoadState(state.patternTableState);
	}
}

void NES::Clock()
{
	// CPU runs every 3 PPU cycles. Running when mod 3 = 2 matches nestest log
	if (m_cycleCount % 3 == 2) {

		m_apu.Clock();
		if (m_apu.IsDMCRequestingSample()) {
			this->SetupDMCDMA();
		}

		if (m_DMCDMACycles > 0) {
			// CPU and OAM DMA are suspended due to DMC DMA
			m_DMCDMACycles--;
		}
		else {
			// CPU is suspended while OAM DMA transfer is occuring
			if (m_OAMDMA.GetCyclesRemaining() == 0) {
				m_cpu.Clock();
			}
			m_OAMDMA.Clock();
		}

	}
	m_ppu.Clock();
	m_cycleCount++;
}


void NES::RunUntilNextCycle() {
	this->Clock();
}

void NES::RunUntilNextInstruction() {
	while (m_cpu.GetState().cyclesRemaining > 1) {
		this->Clock();
	}
	while (m_cpu.GetState().cyclesRemaining == 1) {
		this->Clock();
	}
}

void NES::RunUntilNextScanline() {
	unsigned int currScanline = m_ppu.GetState().scanline;
	while (m_ppu.GetState().scanline == currScanline) {
		this->Clock();
	}
}

void NES::RunUntilNextFrame() {
	unsigned int currFrame = m_ppu.GetFrameCount();
	while (currFrame == m_ppu.GetFrameCount()) {
		this->Clock();
	}
}

float NES::GetAudioSample() {
	return m_apu.GetAudioSample();
}

bool NES::PRGRAMNeedsSaving() {
	return m_cartridge != nullptr && m_cartridge->PRGRAMNeedsSaving();
}

std::vector<uint8_t>& NES::GetPRGRAM() {
	assert(m_cartridge != nullptr);
	return m_cartridge->GetPRGRAM();
}

void NES::LoadPRGRAM(const std::vector<uint8_t>& PRGRAMContent) {
	assert(m_cartridge != nullptr);
	m_cartridge->LoadPRGRAM(PRGRAMContent);
}

uint8_t NES::ProbeCPUBus(uint16_t address) {
	return m_cpuBus.Probe(address);
}

uint8_t NES::ProbePPUBus(uint16_t address) {
	return 0;
}

CPUState NES::ProbeCPUState() {
	return this->GetState().cpuState;
}

std::vector<uint8_t> NES::ProbeCurrentCPUInstruction() {
	return m_cpu.GetCurrentInstructionBytes();
}

RAMState NES::ProbeRAMState() {
	return this->GetState().ramState;
}

CartridgeState NES::ProbeCartridgeState() {
	return this->GetState().cartridgeState;
}

std::vector<uint8_t> NES::ProbeCurrentPRGRom() {
	return {};
}

std::vector<uint8_t> NES::ProbeCurrentCHRRom() {
	return {};
}


void NES::ConnectCartridge() {
	assert(m_cartridge != nullptr);
	m_cpuBus.ConnectDevice(m_cartridge);
	m_patternTables.ConnectCartridge(m_cartridge);
	m_nametables.ConnectCartridge(m_cartridge);
}

void NES::DisconnectCartridge() {
	if (m_cartridge != nullptr) {
		m_patternTables.DisconnectCartridge();
		m_nametables.DisconnectCartridge();
		m_cpuBus.DisconnectDevice(m_cartridge);
		delete m_cartridge;
		m_cartridge = nullptr;
	}
}

void NES::SetupDMCDMA() {
	switch (m_OAMDMA.GetCyclesRemaining()) {
	case 0:
		m_DMCDMACycles = 4;
		break;
	case 1:
		m_DMCDMACycles = 3;
		break;
	case 2:
		m_DMCDMACycles = 1;
		break;
	default:
		m_DMCDMACycles = 2;
	}
}