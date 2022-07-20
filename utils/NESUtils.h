#pragma once

#include <string>
#include <cstdint>
#include <vector>


namespace NESUtils {
	std::string HexUint8ToString(uint8_t num);
	std::string DecUint8ToString(uint8_t num);
	std::string HexUint16ToString(uint16_t num);

	class NESTestInstructionLine {
    public:
        NESTestInstructionLine();
        void SetRegisters(uint16_t PC, uint8_t A, uint8_t X, uint8_t Y, uint8_t P, uint8_t SP, uint8_t ram0002value);
        void SetCycles(unsigned int scanline, unsigned int dot, unsigned int numCPUCycles);
        void SetInstructionBytes(std::vector<uint8_t> instructionBytes);
        std::string ToString() const;

    private:
        uint16_t m_PC;
        uint8_t m_A;
        uint8_t m_X;
        uint8_t m_Y;
        uint8_t m_P;
        uint8_t m_SP;
        uint8_t m_ram0002value;
        std::vector<uint8_t> m_instructionBytes;
        unsigned int m_scanline;
        unsigned int m_dot;
        unsigned int m_numCPUCycles;
	};

    void RunCPUTest(std::string romPath, std::string outputPath);
}