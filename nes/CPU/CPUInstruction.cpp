#include "CPUInstruction.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;


std::array<CPUInstruction, OPCODE_TABLE_SIZE> CPU2A03::s_opCodeTable = {
	CPUInstruction((uint8_t)0x00, INSTR_BRK, MODE_IMPLIED, 7),
	CPUInstruction((uint8_t)0x01, INSTR_ORA, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0x02, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x03, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x04, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x05, INSTR_ORA, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x06, INSTR_ASL, MODE_ZERO_PAGE, 5),
	CPUInstruction((uint8_t)0x07, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x08, INSTR_PHP, MODE_IMPLIED, 3),
	CPUInstruction((uint8_t)0x09, INSTR_ORA, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0x0A, INSTR_ASL, MODE_ACCUMULATOR, 2),
	CPUInstruction((uint8_t)0x0B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x0C, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x0D, INSTR_ORA, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x0E, INSTR_ASL, MODE_ABSOLUTE, 6),
	CPUInstruction((uint8_t)0x0F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x10, INSTR_BPL, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0x11, INSTR_ORA, MODE_INDIRECT_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0x12, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x13, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x14, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x15, INSTR_ORA, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x16, INSTR_ASL, MODE_ZERO_PAGE_INDEXED_X, 6),
	CPUInstruction((uint8_t)0x17, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x18, INSTR_CLC, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x19, INSTR_ORA, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0x1A, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x1B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x1C, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x1D, INSTR_ORA, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x1E, INSTR_ASL, MODE_ABSOLUTE_INDEXED_X, 7),
	CPUInstruction((uint8_t)0x1F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x20, INSTR_JSR, MODE_ABSOLUTE, 6),
	CPUInstruction((uint8_t)0x21, INSTR_AND, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0x22, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x23, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x24, INSTR_BIT, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x25, INSTR_AND, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x26, INSTR_ROL, MODE_ZERO_PAGE, 5),
	CPUInstruction((uint8_t)0x27, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x28, INSTR_PLP, MODE_IMPLIED, 4),
	CPUInstruction((uint8_t)0x29, INSTR_AND, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0x2A, INSTR_ROL, MODE_ACCUMULATOR, 2),
	CPUInstruction((uint8_t)0x2B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x2C, INSTR_BIT, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x2D, INSTR_AND, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x2E, INSTR_ROL, MODE_ABSOLUTE, 6),
	CPUInstruction((uint8_t)0x2F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x30, INSTR_BMI, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0x31, INSTR_AND, MODE_INDIRECT_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0x32, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x33, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x34, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x35, INSTR_AND, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x36, INSTR_ROL, MODE_ZERO_PAGE_INDEXED_X, 6),
	CPUInstruction((uint8_t)0x37, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x38, INSTR_SEC, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x39, INSTR_AND, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0x3A, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x3B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x3C, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x3D, INSTR_AND, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x3E, INSTR_ROL, MODE_ABSOLUTE_INDEXED_X, 7),
	CPUInstruction((uint8_t)0x3F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x40, INSTR_RTI, MODE_IMPLIED, 6),
	CPUInstruction((uint8_t)0x41, INSTR_EOR, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0x42, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x43, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x44, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x45, INSTR_EOR, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x46, INSTR_LSR, MODE_ZERO_PAGE, 5),
	CPUInstruction((uint8_t)0x47, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x48, INSTR_PHA, MODE_IMPLIED, 3),
	CPUInstruction((uint8_t)0x49, INSTR_EOR, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0x4A, INSTR_LSR, MODE_ACCUMULATOR, 2),
	CPUInstruction((uint8_t)0x4B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x4C, INSTR_JMP, MODE_ABSOLUTE, 3),
	CPUInstruction((uint8_t)0x4D, INSTR_EOR, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x4E, INSTR_LSR, MODE_ABSOLUTE, 6),
	CPUInstruction((uint8_t)0x4F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x50, INSTR_BVC, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0x51, INSTR_EOR, MODE_INDIRECT_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0x52, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x53, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x54, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x55, INSTR_EOR, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x56, INSTR_LSR, MODE_ZERO_PAGE_INDEXED_X, 6),
	CPUInstruction((uint8_t)0x57, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x58, INSTR_CLI, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x59, INSTR_EOR, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0x5A, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x5B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x5C, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x5D, INSTR_EOR, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x5E, INSTR_LSR, MODE_ABSOLUTE_INDEXED_X, 7),
	CPUInstruction((uint8_t)0x5F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x60, INSTR_RTS, MODE_IMPLIED, 6),
	CPUInstruction((uint8_t)0x61, INSTR_ADC, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0x62, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x63, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x64, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x65, INSTR_ADC, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x66, INSTR_ROR, MODE_ZERO_PAGE, 5),
	CPUInstruction((uint8_t)0x67, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x68, INSTR_PLA, MODE_IMPLIED, 4),
	CPUInstruction((uint8_t)0x69, INSTR_ADC, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0x6A, INSTR_ROR, MODE_ACCUMULATOR, 2),
	CPUInstruction((uint8_t)0x6B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x6C, INSTR_JMP, MODE_INDIRECT, 5),
	CPUInstruction((uint8_t)0x6D, INSTR_ADC, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x6E, INSTR_ROR, MODE_ABSOLUTE, 6),
	CPUInstruction((uint8_t)0x6F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x70, INSTR_BVS, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0x71, INSTR_ADC, MODE_INDIRECT_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0x72, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x73, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x74, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x75, INSTR_ADC, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x76, INSTR_ROR, MODE_ZERO_PAGE_INDEXED_X, 6),
	CPUInstruction((uint8_t)0x77, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x78, INSTR_SEI, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x79, INSTR_ADC, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0x7A, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x7B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x7C, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x7D, INSTR_ADC, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x7E, INSTR_ROR, MODE_ABSOLUTE_INDEXED_X, 7),
	CPUInstruction((uint8_t)0x7F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x80, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x81, INSTR_STA, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0x82, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x83, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x84, INSTR_STY, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x85, INSTR_STA, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x86, INSTR_STX, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0x87, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x88, INSTR_DEY, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x89, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x8A, INSTR_TXA, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x8B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x8C, INSTR_STY, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x8D, INSTR_STA, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x8E, INSTR_STX, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0x8F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0x90, INSTR_BCC, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0x91, INSTR_STA, MODE_INDIRECT_INDEXED_Y, 6),
	CPUInstruction((uint8_t)0x92, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x93, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x94, INSTR_STY, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x95, INSTR_STA, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0x96, INSTR_STX, MODE_ZERO_PAGE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0x97, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x98, INSTR_TYA, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x99, INSTR_STA, MODE_ABSOLUTE_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0x9A, INSTR_TXS, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0x9B, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x9C, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x9D, INSTR_STA, MODE_ABSOLUTE_INDEXED_X, 5),
	CPUInstruction((uint8_t)0x9E, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0x9F, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0xA0, INSTR_LDY, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0xA1, INSTR_LDA, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0xA2, INSTR_LDX, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0xA3, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xA4, INSTR_LDY, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0xA5, INSTR_LDA, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0xA6, INSTR_LDX, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0xA7, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xA8, INSTR_TAY, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xA9, INSTR_LDA, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0xAA, INSTR_TAX, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xAB, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xAC, INSTR_LDY, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0xAD, INSTR_LDA, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0xAE, INSTR_LDX, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0xAF, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0xB0, INSTR_BCS, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0xB1, INSTR_LDA, MODE_INDIRECT_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0xB2, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xB3, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xB4, INSTR_LDY, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xB5, INSTR_LDA, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xB6, INSTR_LDX, MODE_ZERO_PAGE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0xB7, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xB8, INSTR_CLV, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xB9, INSTR_LDA, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0xBA, INSTR_TSX, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xBB, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xBC, INSTR_LDY, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xBD, INSTR_LDA, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xBE, INSTR_LDX, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0xBF, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0xC0, INSTR_CPY, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0xC1, INSTR_CMP, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0xC2, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xC3, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xC4, INSTR_CPY, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0xC5, INSTR_CMP, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0xC6, INSTR_DEC, MODE_ZERO_PAGE, 5),
	CPUInstruction((uint8_t)0xC7, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xC8, INSTR_INY, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xC9, INSTR_CMP, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0xCA, INSTR_DEX, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xCB, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xCC, INSTR_CPY, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0xCD, INSTR_CMP, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0xCE, INSTR_DEC, MODE_ABSOLUTE, 6),
	CPUInstruction((uint8_t)0xCF, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0xD0, INSTR_BNE, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0xD1, INSTR_CMP, MODE_INDIRECT_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0xD2, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xD3, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xD4, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xD5, INSTR_CMP, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xD6, INSTR_DEC, MODE_ZERO_PAGE_INDEXED_X, 6),
	CPUInstruction((uint8_t)0xD7, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xD8, INSTR_CLD, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xD9, INSTR_CMP, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0xDA, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xDB, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xDC, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xDD, INSTR_CMP, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xDE, INSTR_DEC, MODE_ABSOLUTE_INDEXED_X, 7),
	CPUInstruction((uint8_t)0xDF, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0xE0, INSTR_CPX, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0xE1, INSTR_SBC, MODE_INDEXED_INDIRECT_X, 6),
	CPUInstruction((uint8_t)0xE2, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xE3, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xE4, INSTR_CPX, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0xE5, INSTR_SBC, MODE_ZERO_PAGE, 3),
	CPUInstruction((uint8_t)0xE6, INSTR_INC, MODE_ZERO_PAGE, 5),
	CPUInstruction((uint8_t)0xE7, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xE8, INSTR_INX, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xE9, INSTR_SBC, MODE_IMMEDIATE, 2),
	CPUInstruction((uint8_t)0xEA, INSTR_NOP, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xEB, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xEC, INSTR_CPX, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0xED, INSTR_SBC, MODE_ABSOLUTE, 4),
	CPUInstruction((uint8_t)0xEE, INSTR_INC, MODE_ABSOLUTE, 6),
	CPUInstruction((uint8_t)0xEF, INSTR_ILLEGAL, MODE_IMPLIED, 1),

	CPUInstruction((uint8_t)0xF0, INSTR_BEQ, MODE_RELATIVE, 2),
	CPUInstruction((uint8_t)0xF1, INSTR_SBC, MODE_INDIRECT_INDEXED_Y, 5),
	CPUInstruction((uint8_t)0xF2, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xF3, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xF4, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xF5, INSTR_SBC, MODE_ZERO_PAGE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xF6, INSTR_INC, MODE_ZERO_PAGE_INDEXED_X, 6),
	CPUInstruction((uint8_t)0xF7, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xF8, INSTR_SED, MODE_IMPLIED, 2),
	CPUInstruction((uint8_t)0xF9, INSTR_SBC, MODE_ABSOLUTE_INDEXED_Y, 4),
	CPUInstruction((uint8_t)0xFA, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xFB, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xFC, INSTR_ILLEGAL, MODE_IMPLIED, 1),
	CPUInstruction((uint8_t)0xFD, INSTR_SBC, MODE_ABSOLUTE_INDEXED_X, 4),
	CPUInstruction((uint8_t)0xFE, INSTR_INC, MODE_ABSOLUTE_INDEXED_X, 7),
	CPUInstruction((uint8_t)0xFF, INSTR_ILLEGAL, MODE_IMPLIED, 1)
};

std::array<InstructionMnemonic, NUM_MNEMONICS_WITHOUT_ADDITIONAL_CYCLES> CPU2A03::s_mnemonicsWithoutAdditionalCycles = {
	INSTR_ASL,
	INSTR_DEC,
	INSTR_INC,
	INSTR_LSR,
	INSTR_ROL,
	INSTR_ROR,
	INSTR_STA
};

std::map<AddressingMode, int> CPUInstruction::s_addressingModeToLength = {
	{MODE_IMPLIED, 1},
	{MODE_ACCUMULATOR, 1},
	{MODE_IMMEDIATE, 2},
	{MODE_ZERO_PAGE, 2},
	{MODE_ABSOLUTE, 3},
	{MODE_RELATIVE, 2},
	{MODE_INDIRECT, 3},
	{MODE_ZERO_PAGE_INDEXED_X, 2},
	{MODE_ZERO_PAGE_INDEXED_Y, 2},
	{MODE_ABSOLUTE_INDEXED_X, 3},
	{MODE_ABSOLUTE_INDEXED_Y, 3},
	{MODE_INDEXED_INDIRECT_X, 2},
	{MODE_INDIRECT_INDEXED_Y, 2}
};


int CPUInstruction::GetInstructionLength() {
	return CPUInstruction::s_addressingModeToLength[addressMode];
}



int CPU2A03::Implied(uint16_t& targetAddress, bool& accumulatorMode) {
	targetAddress = 0;
	accumulatorMode = false;
	return 0;
}

int CPU2A03::Accumulator(uint16_t& targetAddress, bool& accumulatorMode) {
	targetAddress = 0;
	accumulatorMode = true;
	return 0;
}

int CPU2A03::Immediate(uint16_t& targetAddress, bool& accumulatorMode) {
	targetAddress = m_regPC;
	Inc16Bit(m_regPC);
	accumulatorMode = false;
	return 0;
}

int CPU2A03::ZeroPage(uint16_t& targetAddress, bool& accumulatorMode) {
	targetAddress = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);
	accumulatorMode = false;
	return 0;
}

int CPU2A03::Absolute(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t addressLow = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);
	uint8_t addressHigh = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	targetAddress = CombineBytes(addressLow, addressHigh);
	accumulatorMode = false;
	return 0;
}

int CPU2A03::Relative(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t relativeAddress = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	targetAddress = AddRelativeAddress(m_regPC, relativeAddress);
	accumulatorMode = false;

	return 0;
}

int CPU2A03::Indirect(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t indirectAddressLow = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);
	uint8_t indirectAddressHigh = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	uint8_t effectiveAddressLow = m_cpuBus->Read(CombineBytes(indirectAddressLow, indirectAddressHigh));
	// 6502 Bug - effective address high is always fetched from the same page, rather than crossing to the next page
	uint8_t effectiveAddressHigh = m_cpuBus->Read(CombineBytes(Add8Bit(indirectAddressLow, 1), indirectAddressHigh));

	targetAddress = CombineBytes(effectiveAddressLow, effectiveAddressHigh);
	accumulatorMode = false;

	return 0;
}

int CPU2A03::ZeroPageIndexedX(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t baseAddress = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	targetAddress = Add8Bit(baseAddress, m_regX);
	accumulatorMode = false;
	return 0;
}

int CPU2A03::ZeroPageIndexedY(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t baseAddress = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	targetAddress = Add8Bit(baseAddress, m_regY);
	accumulatorMode = false;
	return 0;
}

int CPU2A03::AbsoluteIndexedX(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t addressLow = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);
	uint8_t addressHigh = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	uint16_t baseAddress = CombineBytes(addressLow, addressHigh);
	targetAddress = Add16Bit(baseAddress, m_regX);
	accumulatorMode = false;

	// add cycle upon page cross
	if (IsUpperByteTheSame(baseAddress, targetAddress)) {
		return 0;
	}
	else {
		return 1;
	}
}

int CPU2A03::AbsoluteIndexedY(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t addressLow = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);
	uint8_t addressHigh = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	uint16_t baseAddress = CombineBytes(addressLow, addressHigh);
	targetAddress = Add16Bit(baseAddress, m_regY);
	accumulatorMode = false;

	// add cycle upon page cross
	if (IsUpperByteTheSame(baseAddress, targetAddress)) {
		return 0;
	}
	else {
		return 1;
	}
}

int CPU2A03::IndexedIndirectX(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t indirectBaseAddress = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	uint8_t indirectAddressLow = Add8Bit(indirectBaseAddress, m_regX);
	uint8_t indirectAddressHigh = Add8Bit(indirectAddressLow, 1);
	uint8_t effectiveAddressLow = m_cpuBus->Read(indirectAddressLow);
	uint8_t effectiveAddressHigh = m_cpuBus->Read(indirectAddressHigh);

	targetAddress = CombineBytes(effectiveAddressLow, effectiveAddressHigh);
	accumulatorMode = false;
	return 0;
}

int CPU2A03::IndirectIndexedY(uint16_t& targetAddress, bool& accumulatorMode) {
	uint8_t indirectBaseAddress = m_cpuBus->Read(m_regPC);
	Inc16Bit(m_regPC);

	uint8_t baseAddressLow = m_cpuBus->Read(indirectBaseAddress);
	uint8_t baseAddressHigh = m_cpuBus->Read(Add8Bit(indirectBaseAddress, 1));
	uint16_t baseAddress = CombineBytes(baseAddressLow, baseAddressHigh);

	targetAddress = Add16Bit(baseAddress, m_regY);
	accumulatorMode = false;

	// add cycle upon page cross
	if (IsUpperByteTheSame(baseAddress, targetAddress)) {
		return 0;
	}
	else {
		return 1;
	}
}



int CPU2A03::Adc(uint16_t targetAddress, bool accumulatorMode) {
	uint8_t value = m_cpuBus->Read(targetAddress);
	if (m_decimalAllowed && this->m_regP.flags.D) {
		return this->AdcDecimalMode(value);
	}
	else {
		return this->AdcBinaryMode(value);
	}
}

int CPU2A03::And(uint16_t targetAddress, bool accumulatorMode)
{
	m_regA &= m_cpuBus->Read(targetAddress);
	this->UpdateZNFlags(m_regA);
	return 0;
}

int CPU2A03::Asl(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value;
	if (accumulatorMode) {
		value = m_regA;
	}
	else {
		value = m_cpuBus->Read(targetAddress);
	}
	m_regP.flags.C = TestBit8(value, 7);
	value = ShiftLeft8(value, 1);
	this->UpdateZNFlags(value);
	if (accumulatorMode) {
		m_regA = value;
	}
	else {
		m_cpuBus->Write(targetAddress, value);
	}
	return 0;
}

int CPU2A03::Bcc(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(!m_regP.flags.C, targetAddress);
}

int CPU2A03::Bcs(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(m_regP.flags.C, targetAddress);
}

int CPU2A03::Beq(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(m_regP.flags.Z, targetAddress);
}

int CPU2A03::Bit(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value = m_cpuBus->Read(targetAddress);
	m_regP.flags.Z = (m_regA & value) == 0;
	m_regP.flags.V = TestBit8(value, 6);
	m_regP.flags.N = TestBit8(value, 7);
	return 0;
}

int CPU2A03::Bmi(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(m_regP.flags.N, targetAddress);
}

int CPU2A03::Bne(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(!m_regP.flags.Z, targetAddress);
}

int CPU2A03::Bpl(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(!m_regP.flags.N, targetAddress);
}

int CPU2A03::Brk(uint16_t targetAddress, bool accumulatorMode)
{
	// For some reason, Brk increases PC once before handling the interrupt. 
	// This means RTI will return to two bytes after BRK, rather than one, as you would expect.
	Inc16Bit(m_regPC);
	HandleInterrupt(true);
	return 0;
}

int CPU2A03::Bvc(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(!m_regP.flags.V, targetAddress);
}

int CPU2A03::Bvs(uint16_t targetAddress, bool accumulatorMode)
{
	return this->Branch(m_regP.flags.V, targetAddress);
}

int CPU2A03::Clc(uint16_t targetAddress, bool accumulatorMode)
{
	m_regP.flags.C = 0;
	return 0;
}

int CPU2A03::Cld(uint16_t targetAddress, bool accumulatorMode)
{
	m_regP.flags.D = 0;
	return 0;
}

int CPU2A03::Cli(uint16_t targetAddress, bool accumulatorMode)
{
	m_regP.flags.I = 0;
	return 0;
}

int CPU2A03::Clv(uint16_t targetAddress, bool accumulatorMode)
{
	m_regP.flags.V = 0;
	return 0;
}

int CPU2A03::Cmp(uint16_t targetAddress, bool accumulatorMode)
{
	return this->CompareRegister(m_regA, m_cpuBus->Read(targetAddress));
}

int CPU2A03::Cpx(uint16_t targetAddress, bool accumulatorMode)
{
	return this->CompareRegister(m_regX, m_cpuBus->Read(targetAddress));
}

int CPU2A03::Cpy(uint16_t targetAddress, bool accumulatorMode)
{
	return this->CompareRegister(m_regY, m_cpuBus->Read(targetAddress));
}

int CPU2A03::Dec(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value = m_cpuBus->Read(targetAddress);
	Dec8Bit(value);
	m_cpuBus->Write(targetAddress, value);
	this->UpdateZNFlags(value);
	return 0;
}

int CPU2A03::Dex(uint16_t targetAddress, bool accumulatorMode)
{
	Dec8Bit(m_regX);
	this->UpdateZNFlags(m_regX);
	return 0;
}

int CPU2A03::Dey(uint16_t targetAddress, bool accumulatorMode)
{
	Dec8Bit(m_regY);
	this->UpdateZNFlags(m_regY);
	return 0;
}

int CPU2A03::Eor(uint16_t targetAddress, bool accumulatorMode)
{
	m_regA ^= m_cpuBus->Read(targetAddress);
	this->UpdateZNFlags(m_regA);
	return 0;
}

int CPU2A03::Inc(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value = m_cpuBus->Read(targetAddress);	
	Inc8Bit(value);
	m_cpuBus->Write(targetAddress, value);
	this->UpdateZNFlags(value);
	return 0;
}

int CPU2A03::Inx(uint16_t targetAddress, bool accumulatorMode)
{
	Inc8Bit(m_regX);
	this->UpdateZNFlags(m_regX);
	return 0;
}

int CPU2A03::Iny(uint16_t targetAddress, bool accumulatorMode)
{
	Inc8Bit(m_regY);
	this->UpdateZNFlags(m_regY);
	return 0;
}

int CPU2A03::Jmp(uint16_t targetAddress, bool accumulatorMode)
{
	m_regPC = targetAddress;
	return 0;
}

int CPU2A03::Jsr(uint16_t targetAddress, bool accumulatorMode)
{
	// The last address of the JSR instruction is stored on the stack, so we must decrement PC first
	Dec16Bit(m_regPC);
	this->PushToStack(ExtractHighByte(m_regPC));
	this->PushToStack(ExtractLowByte(m_regPC));
	m_regPC = targetAddress;
	return 0;
}

int CPU2A03::Lda(uint16_t targetAddress, bool accumulatorMode)
{
	m_regA = m_cpuBus->Read(targetAddress);
	this->UpdateZNFlags(m_regA);
	return 0;
}

int CPU2A03::Ldx(uint16_t targetAddress, bool accumulatorMode)
{
	m_regX = m_cpuBus->Read(targetAddress);
	this->UpdateZNFlags(m_regX);
	return 0;
}

int CPU2A03::Ldy(uint16_t targetAddress, bool accumulatorMode)
{
	m_regY = m_cpuBus->Read(targetAddress);
	this->UpdateZNFlags(m_regY);
	return 0;
}

int CPU2A03::Lsr(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value;
	if (accumulatorMode) {
		value = m_regA;
	}
	else {
		value = m_cpuBus->Read(targetAddress);
	}
	m_regP.flags.C = TestBit8(value, 0);
	value = ShiftRight8(value, 1);
	this->UpdateZNFlags(value);
	if (accumulatorMode) {
		m_regA = value;
	}
	else {
		m_cpuBus->Write(targetAddress, value);
	}
	return 0;
}

int CPU2A03::Nop(uint16_t targetAddress, bool accumulatorMode)
{
	return 0;
}

int CPU2A03::Ora(uint16_t targetAddress, bool accumulatorMode)
{
	m_regA |= m_cpuBus->Read(targetAddress);
	this->UpdateZNFlags(m_regA);
	return 0;
}

int CPU2A03::Pha(uint16_t targetAddress, bool accumulatorMode)
{
	this->PushToStack(m_regA);
	return 0;
}

int CPU2A03::Php(uint16_t targetAddress, bool accumulatorMode)
{
	// B and Unused should be set when pushed into stack
	CPUStatusRegister pToPush = m_regP;
	pToPush.flags.B = 1;
	pToPush.flags.Unused = 1;
	this->PushToStack(pToPush.value);
	return 0;
}

int CPU2A03::Pla(uint16_t targetAddress, bool accumulatorMode)
{
	m_regA = this->PullFromStack();
	this->UpdateZNFlags(m_regA);
	return 0;
}

int CPU2A03::Plp(uint16_t targetAddress, bool accumulatorMode)
{
	// B and Unused flags are ignored when pulled
	CPUStatusRegister pPulled;
	pPulled.value = this->PullFromStack();
	pPulled.flags.B = 0;
	pPulled.flags.Unused = 1;
	m_regP = pPulled;
	return 0;
}

int CPU2A03::Rol(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value;
	if (accumulatorMode) {
		value = m_regA;
	}
	else {
		value = m_cpuBus->Read(targetAddress);
	}

	bool newCarry = TestBit8(value, 7);
	value = ShiftLeft8(value, 1);
	if (m_regP.flags.C) {
		SetBit8(value, 0);
	}
	m_regP.flags.C = newCarry;
	this->UpdateZNFlags(value);

	if (accumulatorMode) {
		m_regA = value;
	}
	else {
		m_cpuBus->Write(targetAddress, value);
	}
	return 0;
}

int CPU2A03::Ror(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value;
	if (accumulatorMode) {
		value = m_regA;
	}
	else {
		value = m_cpuBus->Read(targetAddress);
	}
	bool newCarry = TestBit8(value, 0);
	value = ShiftRight8(value, 1);
	if (m_regP.flags.C) {
		SetBit8(value, 7);
	}
	m_regP.flags.C = newCarry;
	this->UpdateZNFlags(value);
	if (accumulatorMode) {
		m_regA = value;
	}
	else {
		m_cpuBus->Write(targetAddress, value);
	}
	return 0;
}

int CPU2A03::Rti(uint16_t targetAddress, bool accumulatorMode)
{
	// B and Unused flags are ignored when pulled from stack
	m_regP.value = this->PullFromStack();
	m_regP.flags.B = 0;
	m_regP.flags.Unused = 1;
	uint8_t pcLow = this->PullFromStack();
	uint8_t pcHigh = this->PullFromStack();
	m_regPC = CombineBytes(pcLow, pcHigh);
	return 0;
}

int CPU2A03::Rts(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t pcLow = this->PullFromStack();
	uint8_t pcHigh = this->PullFromStack();
	m_regPC = CombineBytes(pcLow, pcHigh);
	// For some reason, PC was decremented once when calling JSR, so now we need to increment it
	Inc16Bit(m_regPC);
	return 0;
}

int CPU2A03::Sbc(uint16_t targetAddress, bool accumulatorMode)
{
	uint8_t value = m_cpuBus->Read(targetAddress);
	if (m_decimalAllowed && this->m_regP.flags.D) {
		return this->SbcDecimalMode(value);
	}
	else {
		return this->SbcBinaryMode(value);
	}
}

int CPU2A03::Sec(uint16_t targetAddress, bool accumulatorMode)
{
	m_regP.flags.C = 1;
	return 0;
}

int CPU2A03::Sed(uint16_t targetAddress, bool accumulatorMode)
{
	m_regP.flags.D = 1;
	return 0;
}

int CPU2A03::Sei(uint16_t targetAddress, bool accumulatorMode)
{
	m_regP.flags.I = 1;
	return 0;
}

int CPU2A03::Sta(uint16_t targetAddress, bool accumulatorMode)
{
	m_cpuBus->Write(targetAddress, m_regA);
	return 0;
}

int CPU2A03::Stx(uint16_t targetAddress, bool accumulatorMode)
{
	m_cpuBus->Write(targetAddress, m_regX);
	return 0;
}

int CPU2A03::Sty(uint16_t targetAddress, bool accumulatorMode)
{
	m_cpuBus->Write(targetAddress, m_regY);
	return 0;
}

int CPU2A03::Tax(uint16_t targetAddress, bool accumulatorMode)
{
	m_regX = m_regA;
	this->UpdateZNFlags(m_regX);
	return 0;
}

int CPU2A03::Tay(uint16_t targetAddress, bool accumulatorMode)
{
	m_regY = m_regA;
	this->UpdateZNFlags(m_regY);
	return 0;
}

int CPU2A03::Tsx(uint16_t targetAddress, bool accumulatorMode)
{
	m_regX = m_regS;
	this->UpdateZNFlags(m_regX);
	return 0;
}

int CPU2A03::Txa(uint16_t targetAddress, bool accumulatorMode)
{
	m_regA = m_regX;
	this->UpdateZNFlags(m_regA);
	return 0;
}

int CPU2A03::Txs(uint16_t targetAddress, bool accumulatorMode)
{
	m_regS = m_regX;
	return 0;
}

int CPU2A03::Tya(uint16_t targetAddress, bool accumulatorMode)
{
	m_regA = m_regY;
	this->UpdateZNFlags(m_regA);
	return 0;
}

int CPU2A03::Illegal(uint16_t targetAddress, bool accumulatorMode)
{
	uint16_t address = m_regPC;
	Dec16Bit(address);
	uint8_t opCode = m_cpuBus->Probe(address);
	throw IllegalInstructionException(address, opCode);
}



void CPU2A03::PushToStack(uint8_t value) {
	m_cpuBus->Write(CombineBytes(m_regS, STACK_PAGE), value);
	Dec8Bit(m_regS);
}

uint8_t CPU2A03::PullFromStack() {
	Inc8Bit(m_regS);
	return m_cpuBus->Read(CombineBytes(m_regS, STACK_PAGE));
}

void CPU2A03::HandleInterrupt(bool initiatedByBrk) {
	// The following code determines which vector to use to handle interrupt
	// This allows NMI to hijack IRQ and BRK, and IRQ to hijack BRK
	// As I understand it, this is the behaviour on the NES, although in this emulation the hijacking behaviour would have no actual effect, due to non-cycle accuracy


	uint16_t vectorLow = 0;
	uint16_t vectorHigh = 0;
	if (m_nmiRaised) {
		vectorLow = ADDR_NMI_VECTOR_LOW;
		vectorHigh = ADDR_NMI_VECTOR_HIGH;
		m_nmiRaised = false;
	}
	else {
		vectorLow = ADDR_IRQ_BRK_VECTOR_LOW;
		vectorHigh = ADDR_IRQ_BRK_VECTOR_HIGH;
		// If there are IRQ pending, assume we are handling those. The programmer might handle the BRK instead
		if (!m_regP.flags.I && m_irqPending > 0) {
			m_irqPending--;
		}
		else if (!initiatedByBrk) {
			// Can't handle IRQ and BRK was not initiated, so don't do anything
			return;
		}
	}

	this->PushToStack(ExtractHighByte(m_regPC));
	this->PushToStack(ExtractLowByte(m_regPC));

	CPUStatusRegister pToPush;
	pToPush.value = m_regP.value;
	pToPush.flags.B = initiatedByBrk;
	pToPush.flags.Unused = 1;
	this->PushToStack(pToPush.value);

	uint8_t newAddrLow = m_cpuBus->Read(vectorLow);
	uint8_t newAddrHigh = m_cpuBus->Read(vectorHigh);
	m_regPC = CombineBytes(newAddrLow, newAddrHigh);
	// I believe the I flag is always set no matter what caused the interrupt, although one source I read online stated NMI doesn't affect I, so not sure
	m_regP.flags.I = true;

	// Number of cycles it takes to perform an interrupt
	m_cyclesRemaining = 7;
}


int CPU2A03::AdcBinaryMode(uint8_t value) {
	bool setCarry = false;
	bool setOverflow = false;

	uint8_t resultTemp = Add8Bit(value, m_regP.flags.C);
	if (resultTemp < value) {
		setCarry = true;
	}

	uint8_t result = Add8Bit(m_regA, resultTemp);
	if (result < m_regA) {
		setCarry = true;
	}

	if ((TestBit8(m_regA, 7) != TestBit8(result, 7)) &&
		(TestBit8(m_regA, 7) == TestBit8(value, 7))) {
		m_regP.flags.V = true;
	}
	else {
		m_regP.flags.V = false;
	}
	
	m_regP.flags.C = setCarry;
	m_regA = result;
	this->UpdateZNFlags(m_regA);

	return 0;
}

int CPU2A03::AdcDecimalMode(uint8_t value) {
	// Based on the code listed in nesdev.org/6502_cpu.txt with minor corrections
	// May not be correct, but the NES isn't supposed to use this anyways

	uint8_t accumulatorLow = (m_regA & 0x0F) + (value & 0x0F) + m_regP.flags.C;
	
	// BCD fixup for lower 4 bits
	if (accumulatorLow > 0x09) {
		accumulatorLow += 0x06;
	}

	uint8_t accumulatorHigh = (m_regA >> 4) + (value >> 4) + (accumulatorLow > 0x0F);


	// Zero flag not affected by D flag.
	m_regP.flags.Z = Add8Bit(m_regA, Add8Bit(value, m_regP.flags.C)) == 0;
	// Not sure how the N flag is set on actual hardware, this just checks MSB
	m_regP.flags.N = ((accumulatorHigh & 0x08) != 0);
	m_regP.flags.V = (((accumulatorHigh << 4) ^ m_regA) & 0x80) && !((m_regA ^ value) & 0x80);

	// BCD fixup for upper 4 bits
	if (accumulatorHigh > 0x09) {
		accumulatorHigh += 0x06;
	}

	m_regP.flags.C = accumulatorHigh > 0x0F;
	m_regA = ((accumulatorHigh << 4) | (accumulatorLow & 0x0F)) & 0xFF;

	return 0;
}

int CPU2A03::SbcBinaryMode(uint8_t value) {
	return this->AdcBinaryMode(~value);
}

int CPU2A03::SbcDecimalMode(uint8_t value) {
	// TODO: Implement?
	return 0;
}

int CPU2A03::Branch(bool shouldBranch, uint16_t targetAddress) {
	int additionalCycles = 0;
	if (shouldBranch) {
		// additional cycle for branching
		additionalCycles += 1;
		// aditional cycle for page crossing while computing new address
		if (ExtractHighByte(targetAddress) != ExtractHighByte(m_regPC)) {
			additionalCycles += 1;
		}
		m_regPC = targetAddress;
	}
	return additionalCycles;
}

int CPU2A03::CompareRegister(uint8_t regValue, uint8_t memoryValue) {
	uint8_t complementedValue = Add8Bit(~memoryValue, 1);
	uint8_t cmpResult = Add8Bit(regValue, complementedValue);
	m_regP.flags.C = (complementedValue == 0x00) || (cmpResult < regValue);
	this->UpdateZNFlags(cmpResult);
	return 0;
}

void CPU2A03::UpdateZNFlags(uint8_t testValue) {
	m_regP.flags.Z = (testValue == 0);
	m_regP.flags.N = TestBit8(testValue, 7);
}

void CPU2A03::PopulateFunctionMaps() {
	m_addressModeFunctions[MODE_IMPLIED] = &CPU2A03::Implied;
	m_addressModeFunctions[MODE_ACCUMULATOR] = &CPU2A03::Accumulator;
	m_addressModeFunctions[MODE_IMMEDIATE] = &CPU2A03::Immediate;
	m_addressModeFunctions[MODE_ZERO_PAGE] = &CPU2A03::ZeroPage;
	m_addressModeFunctions[MODE_ABSOLUTE] = &CPU2A03::Absolute;
	m_addressModeFunctions[MODE_RELATIVE] = &CPU2A03::Relative;
	m_addressModeFunctions[MODE_INDIRECT] = &CPU2A03::Indirect;
	m_addressModeFunctions[MODE_ZERO_PAGE_INDEXED_X] = &CPU2A03::ZeroPageIndexedX;
	m_addressModeFunctions[MODE_ZERO_PAGE_INDEXED_Y] = &CPU2A03::ZeroPageIndexedY;
	m_addressModeFunctions[MODE_ABSOLUTE_INDEXED_X] = &CPU2A03::AbsoluteIndexedX;
	m_addressModeFunctions[MODE_ABSOLUTE_INDEXED_Y] = &CPU2A03::AbsoluteIndexedY;
	m_addressModeFunctions[MODE_INDEXED_INDIRECT_X] = &CPU2A03::IndexedIndirectX;
	m_addressModeFunctions[MODE_INDIRECT_INDEXED_Y] = &CPU2A03::IndirectIndexedY;

	m_executeFunctions[INSTR_ADC] = &CPU2A03::Adc;
	m_executeFunctions[INSTR_AND] = &CPU2A03::And;
	m_executeFunctions[INSTR_ASL] = &CPU2A03::Asl;
	m_executeFunctions[INSTR_BCC] = &CPU2A03::Bcc;
	m_executeFunctions[INSTR_BCS] = &CPU2A03::Bcs;
	m_executeFunctions[INSTR_BEQ] = &CPU2A03::Beq;
	m_executeFunctions[INSTR_BIT] = &CPU2A03::Bit;
	m_executeFunctions[INSTR_BMI] = &CPU2A03::Bmi;
	m_executeFunctions[INSTR_BNE] = &CPU2A03::Bne;
	m_executeFunctions[INSTR_BPL] = &CPU2A03::Bpl;
	m_executeFunctions[INSTR_BRK] = &CPU2A03::Brk;
	m_executeFunctions[INSTR_BVC] = &CPU2A03::Bvc;
	m_executeFunctions[INSTR_BVS] = &CPU2A03::Bvs;
	m_executeFunctions[INSTR_CLC] = &CPU2A03::Clc;
	m_executeFunctions[INSTR_CLD] = &CPU2A03::Cld;
	m_executeFunctions[INSTR_CLI] = &CPU2A03::Cli;
	m_executeFunctions[INSTR_CLV] = &CPU2A03::Clv;
	m_executeFunctions[INSTR_CMP] = &CPU2A03::Cmp;
	m_executeFunctions[INSTR_CPX] = &CPU2A03::Cpx;
	m_executeFunctions[INSTR_CPY] = &CPU2A03::Cpy;
	m_executeFunctions[INSTR_DEC] = &CPU2A03::Dec;
	m_executeFunctions[INSTR_DEX] = &CPU2A03::Dex;
	m_executeFunctions[INSTR_DEY] = &CPU2A03::Dey;
	m_executeFunctions[INSTR_EOR] = &CPU2A03::Eor;
	m_executeFunctions[INSTR_INC] = &CPU2A03::Inc;
	m_executeFunctions[INSTR_INX] = &CPU2A03::Inx;
	m_executeFunctions[INSTR_INY] = &CPU2A03::Iny;
	m_executeFunctions[INSTR_JMP] = &CPU2A03::Jmp;
	m_executeFunctions[INSTR_JSR] = &CPU2A03::Jsr;
	m_executeFunctions[INSTR_LDA] = &CPU2A03::Lda;
	m_executeFunctions[INSTR_LDX] = &CPU2A03::Ldx;
	m_executeFunctions[INSTR_LDY] = &CPU2A03::Ldy;
	m_executeFunctions[INSTR_LSR] = &CPU2A03::Lsr;
	m_executeFunctions[INSTR_NOP] = &CPU2A03::Nop;
	m_executeFunctions[INSTR_ORA] = &CPU2A03::Ora;
	m_executeFunctions[INSTR_PHA] = &CPU2A03::Pha;
	m_executeFunctions[INSTR_PHP] = &CPU2A03::Php;
	m_executeFunctions[INSTR_PLA] = &CPU2A03::Pla;
	m_executeFunctions[INSTR_PLP] = &CPU2A03::Plp;
	m_executeFunctions[INSTR_ROL] = &CPU2A03::Rol;
	m_executeFunctions[INSTR_ROR] = &CPU2A03::Ror;
	m_executeFunctions[INSTR_RTI] = &CPU2A03::Rti;
	m_executeFunctions[INSTR_RTS] = &CPU2A03::Rts;
	m_executeFunctions[INSTR_SBC] = &CPU2A03::Sbc;
	m_executeFunctions[INSTR_SEC] = &CPU2A03::Sec;
	m_executeFunctions[INSTR_SED] = &CPU2A03::Sed;
	m_executeFunctions[INSTR_SEI] = &CPU2A03::Sei;
	m_executeFunctions[INSTR_STA] = &CPU2A03::Sta;
	m_executeFunctions[INSTR_STX] = &CPU2A03::Stx;
	m_executeFunctions[INSTR_STY] = &CPU2A03::Sty;
	m_executeFunctions[INSTR_TAX] = &CPU2A03::Tax;
	m_executeFunctions[INSTR_TAY] = &CPU2A03::Tay;
	m_executeFunctions[INSTR_TSX] = &CPU2A03::Tsx;
	m_executeFunctions[INSTR_TXA] = &CPU2A03::Txa;
	m_executeFunctions[INSTR_TXS] = &CPU2A03::Txs;
	m_executeFunctions[INSTR_TYA] = &CPU2A03::Tya;
	m_executeFunctions[INSTR_ILLEGAL] = &CPU2A03::Illegal;
}