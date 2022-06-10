#pragma once

#include <cstdint>
#include <array>
#include <map>
#include <functional>
#include "Bus.h"
#include "CPUInstruction.h"

constexpr uint8_t STACK_PAGE = 0x01;
constexpr uint8_t SP_INITIAL_VALUE = 0xFD;

// Memory locations to fetch PC values from
// upon certain events
constexpr uint16_t ADDR_NMI_VECTOR_LOW = 0xFFFA;
constexpr uint16_t ADDR_NMI_VECTOR_HIGH = 0xFFFB;
constexpr uint16_t ADDR_RESET_VECTOR_LOW = 0x400;// 0xFFFC;
constexpr uint16_t ADDR_RESET_VECTOR_HIGH = 0x401;// 0xFFFD;
constexpr uint16_t ADDR_IRQ_BRK_VECTOR_LOW = 0xFFFE;
constexpr uint16_t ADDR_IRQ_BRK_VECTOR_HIGH = 0xFFFF;

constexpr int OPCODE_TABLE_SIZE = 256;
constexpr int NUM_MNEMONICS_WITHOUT_ADDITIONAL_CYCLES = 7;

constexpr int NUM_RESET_CYCLES = 6;


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

struct CPUInstruction;
enum Instruction_Mnemonic;
enum AddressingMode;


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

	void RaiseIRQ();
	void RaiseNMI();


	static std::array<CPUInstruction, OPCODE_TABLE_SIZE> s_opCodeTable;
	static std::array<Instruction_Mnemonic, NUM_MNEMONICS_WITHOUT_ADDITIONAL_CYCLES> s_MnemonicsWithoutAdditionalCycles;

private:
	// Address mode functions
	// These return parameters that the instruction execution function needs. Different instructions need different parameters, so not all three are used at once.
	// value - data for the instruction to execute upon (often the content of memory). If no data is required - this will be set to 0
	// targetAddress - some instructions have a destination in memory. If the destination is a register instead - this will be set to 0
	// accumulatorMode - ASL, LSR, ROR, ROL need to know if they are in accumulator mode. If true, will operate on the accumulator, else on targetAddress.
	// Return value is 1 if an additional cycle is needed due to page crossing, else 0.
	int Implied(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int Accumulator(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int Immediate(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int ZeroPage(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int Absolute(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int Relative(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int Indirect(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int ZeroPageIndexedX(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int ZeroPageIndexedY(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int AbsoluteIndexedX(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int AbsoluteIndexedY(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int IndexedIndirectX(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);
	int IndirectIndexedY(uint8_t& value, uint16_t& targetAddress, bool& accumulatorMode);



	// Instruction execution functions
	int Adc(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int And(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Asl(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bcc(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bcs(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Beq(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bit(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bmi(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bne(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bpl(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Brk(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bvc(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Bvs(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Clc(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Cld(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Cli(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Clv(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Cmp(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Cpx(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Cpy(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Dec(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Dex(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Dey(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Eor(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Inc(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Inx(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Iny(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Jmp(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Jsr(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Lda(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Ldx(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Ldy(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Lsr(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Nop(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Ora(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Pha(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Php(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Pla(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Plp(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Rol(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Ror(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Rti(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Rts(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Sbc(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Sec(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Sed(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Sei(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Sta(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Stx(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Sty(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Tax(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Tay(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Tsx(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Txa(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Txs(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Tya(uint8_t value, uint16_t targetAddress, bool accumulatorMode);
	int Illegal(uint8_t value, uint16_t targetAddress, bool accumulatorMode);

	void PushToStack(uint8_t value);
	uint8_t PullFromStack();
	void HandleInterrupt(bool initiatedByBrk);

	int AdcBinaryMode(uint8_t value);
	int AdcDecimalMode(uint8_t value);
	int SbcBinaryMode(uint8_t value);
	int SbcDecimalMode(uint8_t value);
	int Branch(bool shouldBranch, uint16_t targetAddress);
	int CompareRegister(uint8_t regValue, uint8_t memoryValue);

	void UpdateZNFlags(uint8_t testValue);
	void PopulateFunctionMaps();

	uint16_t FetchInitialPC();

	Bus* m_cpuBus;

	uint8_t m_regA;
	uint8_t m_regX;
	uint8_t m_regY;
	uint8_t m_regS;
	uint16_t m_regPC;
	CPUStatusRegister m_regP;

	int m_cyclesRemaining;
	int m_irqRequestsPending;
	bool m_nmiRaised;


	std::map<Instruction_Mnemonic, std::function<int(CPU2A03&, uint8_t, uint16_t, bool)> > m_executeFunctions;
	std::map<AddressingMode, std::function<int(CPU2A03&, uint8_t&, uint16_t&, bool&)> > m_addressModeFunctions;

};
