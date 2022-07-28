#pragma once

#include <exception>

#include "../Environment.h"
#include "Bus.h"
#include "CPU/CPU2A03.h"
#include "RAMDevice.h"
#include "PPU/PPU2C02.h"
#include "PPU/PaletteRAMDevice.h"
#include "PPU/PatternTableDevice.h"
#include "ROM/INESFile.h"
#include "mappers/Cartridge.h"



struct NESState {
	CPUState cpuState;
	RAMState ramState;
	CartridgeState cartridgeState;
	PaletteRAMState paletteRAMState;
	PatternTableState patternTableState;
	PPUState ppuState;
};


class NES
{
public:
	NES(Environment* environment = nullptr);
	~NES();

	// Don't allow copies of NES emulator
	NES(const NES&) = delete;
	NES& operator=(const NES&) = delete;

	void LoadROM(const INESFile& romFile);

	void SoftReset();
	void HardReset();

	NESState GetState() const;
	void LoadState(NESState& state);

	void Clock();

	void RunUntilNextCycle();
	void RunUntilNextInstruction();
	void RunUntilNextFrame();

	// Functions for debugging
	uint8_t ProbeCPUBus(uint16_t address);
	uint8_t ProbePPUBus(uint16_t address);
	CPUState ProbeCPUState();
	std::vector<uint8_t> ProbeCurrentCPUInstruction();
	RAMState ProbeRAMState();
	CartridgeState ProbeCartridgeState();
	std::vector<uint8_t> ProbeCurrentPRGRom();
	std::vector<uint8_t> ProbeCurrentCHRRom();

private:
	void ConnectCartridge();
	void DisconnectCartridge();

	Bus m_cpuBus;
	Bus m_ppuBus;
	RAMDevice m_RAM;
	CPU2A03 m_cpu;
	PPU2C02 m_ppu;
	PaletteRAMDevice m_paletteRAM;
	PatternTableDevice m_patternTables;
	Cartridge* m_cartridge;

	unsigned int m_cycleCount;
};

