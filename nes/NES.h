#pragma once

#include <exception>

#include "../Environment.h"
#include "Bus.h"
#include "CPU/CPU2A03.h"
#include "RAMDevice.h"
#include "PPU/PPU2C02.h"
#include "PPU/PaletteRAMDevice.h"
#include "PPU/PatternTableDevice.h"
#include "PPU/NametableDevice.h"
#include "PPU/OAMDMADevice.h"
#include "ControllerDevice.h"
#include "APU/APU2A03.h"
#include  "FrameCounterController2Device.h"
#include "ROM/INESFile.h"
#include "mappers/Cartridge.h"



struct NESState {
	CPUState cpuState;
	RAMState ramState;
	CartridgeState cartridgeState;
	PaletteRAMState paletteRAMState;
	PatternTableState patternTableState;
	NametableState nametableState;
	OAMDMAState oamDMAState;
	PPUState ppuState;
	ControllerState controllerState;
	APUState apuState;
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
	void RunUntilNextScanline();
	void RunUntilNextFrame();

	float GetAudioSample();

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
	NametableDevice m_nametables;
	OAMDMADevice m_OAMDMA;
	ControllerDevice m_controllers;
	APU2A03 m_apu;
	FrameCounterController2Device m_frameCounterController2;
	Cartridge* m_cartridge;

	unsigned int m_cycleCount;
};

