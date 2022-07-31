#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <deque>


namespace NESUtils {
	std::string HexUint8ToString(uint8_t num);
	std::string DecUint8ToString(uint8_t num);
	std::string HexUint16ToString(uint16_t num);
    std::string IntToString(int num, unsigned int minWidth);

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

    template<typename T> class ShiftRegister {
    public:
        ShiftRegister(size_t size) {
            m_size = size;
            m_endIndex = 0;
            m_register.resize(size);
        }

        T& operator[](size_t index) {
            return m_register[index];
        }

        void Shift() {
            if (m_endIndex > 0) {
                m_register.pop_front();
                m_endIndex--;
                m_register.push_back(T());
            }   
        }

        void InsertElements(std::vector<T>& elements) {
            for (T& element : elements) {
                if (m_endIndex == m_size) {
                    // Don't push elements in more than register size
                    return;
                }
                m_register[m_endIndex] = element;
                m_endIndex++;
            }
        }

        void Clear() {
            m_register.clear();
            m_register.resize(m_size);
            m_endIndex = 0;
        }


    private:
        std::deque<T> m_register;
        size_t m_endIndex;
        size_t m_size;
    };

}