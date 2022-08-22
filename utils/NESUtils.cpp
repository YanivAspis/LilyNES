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

	std::string IntToString(int num, unsigned int minWidth) {
		std::stringstream result;
		result << std::setfill('0') << std::setw(minWidth) << num;
		return result.str();
	}


	// The code below is for handling the nestest.nes ROM for testing the CPU implementation

	NESTestInstructionLine::NESTestInstructionLine() {
		this->SetRegisters(0, 0, 0, 0, 0, 0, 0);
		this->SetCycles(0, 0, 0);
	}

	void NESTestInstructionLine::SetRegisters(uint16_t PC, uint8_t A, uint8_t X, uint8_t Y, uint8_t P, uint8_t SP, uint8_t ram0002value) {
		m_PC = PC;
		m_A = A;
		m_X = X;
		m_Y = Y;
		m_P = P;
		m_SP = SP;
		m_ram0002value = ram0002value;
	}

	void NESTestInstructionLine::SetCycles(unsigned int scanline, unsigned int dot, unsigned int numCPUCycles) {
		m_scanline = scanline;
		m_dot = dot;
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
		result += " (" + std::to_string(m_scanline) + "," + std::to_string(m_dot) + ")";
		result += "  Cycles: " + std::to_string(m_numCPUCycles);

		return result;
	}

	void RunCPUTest(std::string romPath, std::string outputPath) {
		INESFile* romFile = new INESFile(romPath);
		NES* nes = new NES();
		nes->LoadROM(*romFile);
		nes->HardReset();

		NESState state = nes->GetState();
		state.cpuState.regPC = 0xC000;
		nes->LoadState(state);

		std::list<NESTestInstructionLine> lines;
		NESTestInstructionLine currInstruction;

		try {
			while (true) {
				nes->RunUntilNextInstruction();
				state = nes->GetState();
				currInstruction.SetRegisters(state.cpuState.regPC, state.cpuState.regA, state.cpuState.regX, state.cpuState.regY, state.cpuState.regP.value, state.cpuState.regS, state.ramState.content[0x0002]);
				currInstruction.SetCycles(state.ppuState.scanline, state.ppuState.dot, state.cpuState.cycleCount);
				currInstruction.SetInstructionBytes(nes->ProbeCurrentCPUInstruction());
				lines.push_back(currInstruction);
			}
		}
		catch (IllegalInstructionException ex) {}

		std::ofstream outputFile;
		outputFile.open(outputPath, std::ios::out | std::ios::trunc);
		for (const NESTestInstructionLine& line : lines) {
			outputFile << line.ToString() << "\n";
		}
		outputFile.close();

		delete romFile;
		delete nes;
	}

	HighPassFilter::HighPassFilter(float cutoffFrequency, float sampleRate) {
		float RC = 1.0 / (2 * M_PI * cutoffFrequency);
		float dt = 1.0 / sampleRate;
		m_alpha = RC / (RC + dt);
		m_firstSampleReceived = false;
		m_lastOriginalSample = 0;
		m_lastResampled = 0;
	}

	void HighPassFilter::Restart() {
		m_firstSampleReceived = false;
		m_lastOriginalSample = 0;
		m_lastResampled = 0;
	}

	float HighPassFilter::Filter(float sample) {
		if (m_firstSampleReceived) {
			m_lastResampled = m_alpha * (m_lastResampled + sample - m_lastOriginalSample);
			m_lastOriginalSample = sample;
		}
		else {
			m_lastResampled = sample;
			m_lastOriginalSample = sample;
			m_firstSampleReceived = true;
		}
		return m_lastResampled;
	}

	LowPassFilter::LowPassFilter(float cutoffFrequency, float sampleRate) {
		float RC = 1.0 / (2 * M_PI * cutoffFrequency);
		float dt = 1.0 / sampleRate;
		m_alpha = dt / (RC + dt);
		m_lastSample = 0;
	}

	void LowPassFilter::Restart() {
		m_lastSample = 0;
	}

	float LowPassFilter::Filter(float sample) {
		m_lastSample += m_alpha * (sample - m_lastSample);
		return m_lastSample;
	}
}
