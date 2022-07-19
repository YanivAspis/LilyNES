#pragma once

#include <exception>

#include "Bus.h"
#include "CPU/CPU2A03.h"
#include "RAMDevice.h"
#include "ROM/INESFile.h"
#include "mappers/Cartridge.h"

struct NESState {
	CPUState cpuState;
	RAMState ramState;
	CartridgeState cartridgeState;
};

class NES
{
public:
	NES();
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

	// TODO: Add arguments for these
	void UpdateDisplay();
	void RequestControllerInput();

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
	RAMDevice m_RAM;
	CPU2A03 m_cpu;
	Cartridge* m_cartridge;
};

