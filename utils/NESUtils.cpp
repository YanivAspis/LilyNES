#include <sstream>
#include <iomanip>
#include <fstream>

#include "NESUtils.h"
#include "../nes/NES.h"
//#include "../nes/CPU/CPU2A03.h"

namespace NESUtils {
	std::string HexUint8ToString(uint8_t num) {
		std::stringstream result;
		result << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (unsigned int)num;
		return result.str();
	}

	std::string DecUint8ToString(uint8_t num) {
		std::stringstream result;
		result << std::setfill('0') << std::setw(3) << (unsigned int)num;
		return result.str();
	}

	std::string HexUint16ToString(uint16_t num) {
		std::stringstream result;
		result << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << (unsigned int)num;
		return result.str();
	}


	// The code below is for handling the nestest.nes ROM for testing the CPU implementation

	NESTestInstructionLine::NESTestInstructionLine() {
		this->SetValues(0, 0, 0, 0, 0, 0, 0, 0);
	}

	void NESTestInstructionLine::SetValues(uint16_t PC, uint8_t A, uint8_t X, uint8_t Y, uint8_t P, uint8_t SP, uint8_t ram0002value, unsigned int numCPUCycles) {
		m_PC = PC;
		m_A = A;
		m_X = X;
		m_Y = Y;
		m_P = P;
		m_SP = SP;
		m_ram0002value = ram0002value;
		m_numCPUCycles = numCPUCycles;
	}

	void NESTestInstructionLine::SetInstructionBytes(std::vector<uint8_t> instructionBytes) {
		m_instructionBytes = instructionBytes;
	}

	std::string NESTestInstructionLine::ToString() const {
		std::string result = HexUint16ToString(m_PC) + "\t";
		for (uint8_t byte : m_instructionBytes) {
			result += HexUint8ToString(byte) + " ";
		}
		result += "\tA: " + HexUint8ToString(m_A);
		result += "  X: " + HexUint8ToString(m_X);
		result += "  Y: " + HexUint8ToString(m_Y);
		result += "  P: " + HexUint8ToString(m_P);
		result += "  SP: " + HexUint8ToString(m_SP);
		result += "  Addr2: " + HexUint8ToString(m_ram0002value);
		result += "  Cycles: " + std::to_string(m_numCPUCycles);

		return result;
	}

	void RunCPUTest(std::string romPath, std::string outputPath) {
		INESFile romFile(romPath);
		NES nes;
		nes.LoadROM(romFile);
		nes.HardReset();

		NESState state = nes.GetState();
		state.cpuState.regPC = 0xC000;
		nes.LoadState(state);

		int numCPUCycles = 0;
		std::list<NESTestInstructionLine> lines;
		NESTestInstructionLine currInstruction;

		try {
			while (true) {
				numCPUCycles++;
				currInstruction.SetValues(state.cpuState.regPC, state.cpuState.regA, state.cpuState.regX, state.cpuState.regY, state.cpuState.regP.value, state.cpuState.regS, state.ramState.content[0x0002], numCPUCycles);
				nes.Clock();
				state = nes.GetState();
				if (state.cpuState.instructionFirstCycle) {
					currInstruction.SetInstructionBytes(nes.ProbeCurrentCPUInstruction());
					lines.push_back(currInstruction);
				}
			}
		}
		catch (IllegalInstructionException ex) {}

		std::ofstream outputFile;
		outputFile.open(outputPath, std::ios::out | std::ios::trunc);
		for (const NESTestInstructionLine& line : lines) {
			outputFile << line.ToString() << "\n";
		}
		outputFile.close();
	}
}
