#pragma once

#include <cstdint>
#include "Bus.h"

constexpr uint8_t SP_INITIAL_VALUE = 0xFD;

// Memory locations to fetch PC values from
// upon certain events
constexpr uint16_t ADDR_NMI_VECTOR_LOW = 0xFFFA;
constexpr uint16_t ADDR_NMI_VECTOR_HIGH = 0xFFFB;
constexpr uint16_t ADDR_RESET_VECTOR_LOW = 0x400;// 0xFFFC;
constexpr uint16_t ADDR_RESET_VECTOR_HIGH = 0x401;// 0xFFFD;
constexpr uint16_t ADDR_IRQ_BRK_VECTOR_LOW = 0xFFFE;
constexpr uint16_t ADDR_IRQ_BRK_VECTOR_HIGH = 0xFFFF;

struct CPUStatusFlag {
	uint8_t C : 1; 		// Carry
	uint8_t Z : 1; 		// Zero
	uint8_t I : 1; 		// Interrupt Disable
	uint8_t D : 1; 		// Decimal mode (unused)
	uint8_t B : 1; 		// Break (unused?)
	uint8_t Unused : 1;
	uint8_t V : 1; 		// Overflow
	uint8_t N : 1; 		// Negative

	void SoftReset();
	void HardReset();
};

union CPUStatusRegister {
	uint8_t value;
	CPUStatusFlag flags;
};


struct CPUState {
	uint8_t regA;
	uint8_t regX;
	uint8_t regY;
	uint8_t regS;
	uint16_t regPC;
	CPUStatusRegister regP;

	CPUState();
};


// The main CPU class

class CPU2A03 {
public:
	CPU2A03();

	void ConnectToBus(Bus* cpuBus);
	void SoftReset();
	void HardReset();

	CPUState GetState() const;
	void LoadState(CPUState& state);

	void Clock();

	void TempTest();

private:
	uint16_t FetchInitialPC();

	Bus* m_cpuBus;

	uint8_t m_regA;
	uint8_t m_regX;
	uint8_t m_regY;
	uint8_t m_regS;
	uint16_t m_regPC;
	CPUStatusRegister m_regP;
};
