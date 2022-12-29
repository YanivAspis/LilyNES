#pragma once

#include <cstdint>
#include <array>
#include <map>
#include <functional>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "../Bus.h"
#include "CPUInstruction.h"

constexpr unsigned int CPU_FREQUENCY = 1786830; // Approximately correct

constexpr uint8_t STACK_PAGE = 0x01;
constexpr uint8_t SP_INITIAL_VALUE = 0xFD;

// Memory locations to fetch PC values from
// upon certain events
constexpr uint16_t ADDR_NMI_VECTOR_LOW = 0xFFFA;
constexpr uint16_t ADDR_NMI_VECTOR_HIGH = 0xFFFB;
constexpr uint16_t ADDR_RESET_VECTOR_LOW = 0xFFFC;
constexpr uint16_t ADDR_RESET_VECTOR_HIGH = 0xFFFD;
constexpr uint16_t ADDR_IRQ_BRK_VECTOR_LOW = 0xFFFE;
constexpr uint16_t ADDR_IRQ_BRK_VECTOR_HIGH = 0xFFFF;

constexpr int OPCODE_TABLE_SIZE = 256;
constexpr int NUM_MNEMONICS_WITHOUT_ADDITIONAL_CYCLES = 7;

constexpr unsigned int NUM_RESET_CYCLES = 7;
constexpr unsigned int NUM_INTERRUPT_CYCLES = 7;

struct CPUInstruction;
enum InstructionMnemonic;
enum AddressingMode;


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

struct CurrentInstruction {
	CurrentInstruction();

	InstructionMnemonic mnemonic;
	AddressingMode addressMode;
	uint16_t instructionAddress;
	uint16_t targetAddress;
	bool accumulatorMode;
	unsigned int length;
	unsigned int cycles;
	bool reset;
	bool interrupt;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& mnemonic;
		ar& addressMode;
		ar& instructionAddress;
		ar& targetAddress;
		ar& accumulatorMode;
		ar& length;
		ar& cycles;
		ar& reset;
		ar& interrupt;
	}
};


struct CPUState {
	uint8_t regA;
	uint8_t regX;
	uint8_t regY;
	uint8_t regS;
	uint16_t regPC;
	CPUStatusRegister regP;

	unsigned int cyclesRemaining;
	std::vector<std::string> irqPending;
	bool nmiRaised;

	bool instructionFirstCycle;
	CurrentInstruction currInstruction;
	unsigned int cycleCount;

	CPUState();

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& regA;
		ar& regX;
		ar& regY;
		ar& regS;
		ar& regPC;
		ar& regP.value;

		ar& cyclesRemaining;
		ar& irqPending;
		ar& nmiRaised;

		ar& instructionFirstCycle;
		ar& currInstruction;
		ar& cycleCount;
	}
};




// The main CPU class

class CPU2A03 {
public:
	CPU2A03(bool decimalAllowed);
	~CPU2A03();
	CPU2A03(const CPU2A03&) = delete;
	CPU2A03& operator=(const CPU2A03&) = delete;

	void ConnectToBus(Bus* cpuBus);
	void SoftReset();
	void HardReset();

	CPUState GetState() const;
	void LoadState(CPUState& state);

	void Clock();

	void RaiseIRQ(std::string irqID);
	void AcknowledgeIRQ(std::string irqID);
	void RaiseNMI();

	std::vector<uint8_t> GetCurrentInstructionBytes();


	static std::array<CPUInstruction, OPCODE_TABLE_SIZE> s_opCodeTable;
	static std::array<InstructionMnemonic, NUM_MNEMONICS_WITHOUT_ADDITIONAL_CYCLES> s_mnemonicsWithoutAdditionalCycles;

private:
	// Address mode functions
	// These emulate the addressing mode by modifying the current instruction values
	// They will update: target address, accumulator mode, additional cycles (due to page cross)
	void Implied();
	void Accumulator();
	void Immediate();
	void ZeroPage();
	void Absolute();
	void Relative();
	void Indirect();
	void ZeroPageIndexedX();
	void ZeroPageIndexedY();
	void AbsoluteIndexedX();
	void AbsoluteIndexedY();
	void IndexedIndirectX();
	void IndirectIndexedY();


	// Instruction execution functions
	void Adc();
	void And();
	void Asl();
	void Bcc();
	void Bcs();
	void Beq();
	void Bit();
	void Bmi();
	void Bne();
	void Bpl();
	void Brk();
	void Bvc();
	void Bvs();
	void Clc();
	void Cld();
	void Cli();
	void Clv();
	void Cmp();
	void Cpx();
	void Cpy();
	void Dec();
	void Dex();
	void Dey();
	void Eor();
	void Inc();
	void Inx();
	void Iny();
	void Jmp();
	void Jsr();
	void Lda();
	void Ldx();
	void Ldy();
	void Lsr();
	void Nop();
	void Ora();
	void Pha();
	void Php();
	void Pla();
	void Plp();
	void Rol();
	void Ror();
	void Rti();
	void Rts();
	void Sbc();
	void Sec();
	void Sed();
	void Sei();
	void Sta();
	void Stx();
	void Sty();
	void Tax();
	void Tay();
	void Tsx();
	void Txa();
	void Txs();
	void Tya();
	void Illegal();

	// Execution additional cycle functions
	void BccCycles();
	void BcsCycles();
	void BeqCycles();
	void BmiCycles();
	void BneCycles();
	void BplCycles();
	void BvcCycles();
	void BvsCycles();

	void PushToStack(uint8_t value);
	uint8_t PullFromStack();
	void HandleInterrupt(bool initiatedByBrk);

	void AdcBinaryMode(uint8_t value);
	void AdcDecimalMode(uint8_t value);
	void SbcBinaryMode(uint8_t value);
	void SbcDecimalMode(uint8_t value);
	void Branch(bool shouldBranch);
	int BranchCycles(bool shouldBranch);
	void CompareRegister(uint8_t regValue, uint8_t memoryValue);

	void UpdateZNFlags(uint8_t testValue);
	void PopulateFunctionMaps();

	uint16_t FetchInitialPC();

	Bus* m_cpuBus;
	// Controls whether decimal mode has any effect. In NES it does not
	bool m_decimalAllowed;

	uint8_t m_regA;
	uint8_t m_regX;
	uint8_t m_regY;
	uint8_t m_regS;
	uint16_t m_regPC;
	CPUStatusRegister m_regP;

	unsigned int m_cyclesRemaining;
	std::vector<std::string> m_irqPending;
	bool m_nmiRaised;

	// True if this is the first cycle for the current instruction. Needed for debugging
	bool m_instructionFirstCycle;
	// Address of current instruction. Needed for debugging
	CurrentInstruction m_currInstruction;
	// Cycle count, needed for debugging
	unsigned int m_cycleCount;


	std::map<InstructionMnemonic, std::function<void(CPU2A03&)> > m_executeFunctions;
	std::map<AddressingMode, std::function<void(CPU2A03&)> > m_addressModeFunctions;
	std::map<InstructionMnemonic, std::function<void(CPU2A03&)> > m_additionalExecuteCyclesFunctions;

};
