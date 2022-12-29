#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <array>
#include <deque>

#include <SDL.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/array.hpp>

struct NESState;

namespace NESUtils {
    class SDLException : public std::exception {
    public:
        SDLException(std::string message) : m_message(message) {
            m_message = message + " Details: " + std::string(SDL_GetError());
        }
        const char* what() const noexcept override {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };



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

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar& m_register;
            ar& m_endIndex;
            ar& m_size;
        }
    };

    class HighPassFilter {
    public:
        HighPassFilter(float cutoffFrequency, float sampleRate);
        void Restart();
        float Filter(float sample);

    private:
        float m_alpha;
        bool m_firstSampleReceived;
        float m_lastOriginalSample;
        float m_lastResampled;
    };

    class LowPassFilter {
    public:
        LowPassFilter(float cutoffFrequency, float sampleRate);
        void Restart();
        float Filter(float sample);

    private:
        float m_alpha;
        float m_lastSample;
    };

    namespace MD5 {
        constexpr size_t CHECKSUM_SIZE = 16;
        constexpr size_t MD5_ROUNDS = 64;
        constexpr size_t CHUNK_SIZE_BITS = 512;
        constexpr size_t CHUNK_SIZE = CHUNK_SIZE_BITS / 8;
        constexpr size_t CHUNK_NUM_DWORDS = CHUNK_SIZE_BITS / 32;
        constexpr size_t MESSAGE_LENGTH_BYTES = 8;

        constexpr std::array<size_t, MD5_ROUNDS> SHIFT_AMOUNTS = {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
            5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
        };

        constexpr std::array<uint32_t, MD5_ROUNDS> CONSTANTS = {
            0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
            0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
            0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
            0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
            0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
            0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
            0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
            0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
            0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
            0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
            0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
            0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
            0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
            0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
            0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
            0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
        };

        constexpr uint32_t INIT_A0 = 0x67452301;
        constexpr uint32_t INIT_B0 = 0xEFCDAB89;
        constexpr uint32_t INIT_C0 = 0x98BADCFE;
        constexpr uint32_t INIT_D0 = 0x10325476;

        std::array<uint8_t, CHECKSUM_SIZE> ComputeChecksum(const std::vector<uint8_t>& message);
        std::string ChecksumToString(const std::array<uint8_t, CHECKSUM_SIZE>& checksum);
        std::vector<uint8_t> PadMessage(const std::vector<uint8_t>& message);
        void ComputeChunkHash(std::array<uint8_t, CHUNK_SIZE>& chunk, uint32_t& a0, uint32_t& b0, uint32_t& c0, uint32_t& d0);
        std::array<uint32_t, CHUNK_NUM_DWORDS> GetDWORDArray(std::array<uint8_t, CHUNK_SIZE>& chunk);
        void ComputeFgValues(unsigned int round, uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t& F, size_t& g);


    }

    void SaveBatteryBackedRAM(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::vector<uint8_t> PRGRAMContent);
    bool LoadBatteryBackedRAM(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::vector<uint8_t>& PRGRAMContent);

    void SaveStateToFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::string filepath, NESState& state);
    void QuickSaveStateToFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, unsigned int slot, NESState& state);
    NESState LoadStateFromFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, std::string filepath);
    NESState QuickLoadStateFromFile(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum, unsigned int slot);
    std::vector<unsigned int> GetSaveStateSlotsAvailable(std::array<uint8_t, MD5::CHECKSUM_SIZE> checksum);
}