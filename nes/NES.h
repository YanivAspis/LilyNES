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
};

class NES
{
public:
	NES();
	~NES();

	// Don't allow copies of NES emulator
	NES(const NES&) = delete;
	NES& operator=(const NES&) = delete;

	void ConnectCartridge(const INESFile& romFile);
	void DisconnectCartridge();

	void SoftReset();
	void HardReset();

	NESState GetState() const;
	void LoadState(NESState& state);

	void Clock();



private:
	Bus m_cpuBus;
	RAMDevice m_RAM;
	CPU2A03 m_cpu;
	Cartridge* m_cartridge;
};

