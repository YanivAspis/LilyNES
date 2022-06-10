#pragma once

#include <cstdint>
#include <exception>
#include "CPU2A03.h"


enum Instruction_Mnemonic {
	INSTR_ADC,
	INSTR_AND,
	INSTR_ASL,
	INSTR_BCC,
	INSTR_BCS,
	INSTR_BEQ,
	INSTR_BIT,
	INSTR_BMI,
	INSTR_BNE,
	INSTR_BPL,
	INSTR_BRK,
	INSTR_BVC,
	INSTR_BVS,
	INSTR_CLC,
	INSTR_CLD,
	INSTR_CLI,
	INSTR_CLV,
	INSTR_CMP,
	INSTR_CPX,
	INSTR_CPY,
	INSTR_DEC,
	INSTR_DEX,
	INSTR_DEY,
	INSTR_EOR,
	INSTR_INC,
	INSTR_INX,
	INSTR_INY,
	INSTR_JMP,
	INSTR_JSR,
	INSTR_LDA,
	INSTR_LDX,
	INSTR_LDY,
	INSTR_LSR,
	INSTR_NOP,
	INSTR_ORA,
	INSTR_PHA,
	INSTR_PHP,
	INSTR_PLA,
	INSTR_PLP,
	INSTR_ROL,
	INSTR_ROR,
	INSTR_RTI,
	INSTR_RTS,
	INSTR_SBC,
	INSTR_SEC,
	INSTR_SED,
	INSTR_SEI,
	INSTR_STA,
	INSTR_STX,
	INSTR_STY,
	INSTR_TAX,
	INSTR_TAY,
	INSTR_TSX,
	INSTR_TXA,
	INSTR_TXS,
	INSTR_TYA,
	INSTR_ILLEGAL
};

enum AddressingMode {
	MODE_IMPLIED,
	MODE_ACCUMULATOR,
	MODE_IMMEDIATE,
	MODE_ZERO_PAGE,
	MODE_ABSOLUTE,
	MODE_RELATIVE,
	MODE_INDIRECT,
	MODE_ZERO_PAGE_INDEXED_X,
	MODE_ZERO_PAGE_INDEXED_Y,
	MODE_ABSOLUTE_INDEXED_X,
	MODE_ABSOLUTE_INDEXED_Y,
	MODE_INDEXED_INDIRECT_X,
	MODE_INDIRECT_INDEXED_Y
};


struct CPUInstruction {
	CPUInstruction(uint8_t opCode, Instruction_Mnemonic mnemonic, AddressingMode addressMode, int baseCycleCount) :
		opCode(opCode), mnemonic(mnemonic), addressMode(addressMode), baseCycleCount(baseCycleCount) {}
	uint8_t opCode;
	Instruction_Mnemonic mnemonic;
	AddressingMode addressMode;
	int baseCycleCount;
};

class IllegalInstructionException : public std::exception {
	const char* what() const throw () {
		return "Illegal instruction detected.";
	}
};

