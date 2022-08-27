#pragma once

#include <cstdint>
#include <array>



namespace BitwiseUtils {

uint8_t TestBit8(uint8_t num, unsigned int bit);
void SetBit8(uint8_t& target, unsigned int bit);
void ClearBit8(uint8_t& target, unsigned int bit);
void ToggleBit8(uint8_t& target, unsigned int bit);
uint8_t ClearLowerBits8(uint8_t target, unsigned int upperBit);
uint8_t ClearUpperBits8(uint8_t target, unsigned int lowerBit);
uint8_t Add8Bit(uint8_t num1, uint8_t num2);
uint8_t ShiftRight8(uint8_t num, unsigned int numBits);
uint8_t ShiftLeft8(uint8_t num, unsigned int numBits);
void Inc8Bit(uint8_t& num);
void Dec8Bit(uint8_t& num);

uint8_t TestBit16(uint16_t num, unsigned int bit);
void SetBit16(uint16_t& target, unsigned int bit);
void ClearBit16(uint16_t& target, unsigned int bit);
void ToggleBit16(uint16_t& target, unsigned int bit);
uint16_t ClearLowerBits16(uint16_t target, unsigned int upperBit);
uint16_t ClearUpperBits16(uint16_t target, unsigned int lowerBit);
uint16_t Add16Bit(uint16_t num1, uint16_t num2);
uint16_t ShiftRight16(uint16_t num, unsigned int numBits);
uint16_t ShiftLeft16(uint16_t num, unsigned int numBits);
void Inc16Bit(uint16_t& num);
void Dec16Bit(uint16_t& num);
uint16_t CombineBytes(uint8_t lowByte, uint8_t highByte);
uint8_t ExtractLowByte(uint16_t word);
uint8_t ExtractHighByte(uint16_t word);

uint16_t AddRelativeAddress(uint16_t baseAddress, uint8_t signedRelativeAddress);
bool IsUpperByteTheSame(uint16_t word1, uint16_t word2);

uint32_t Add32Bit(uint32_t num1, uint32_t num2);
uint32_t CombineBytes32(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
std::array<uint8_t, 4> SplitBytes32(uint32_t num);
uint32_t RotateLeft32Bit(uint32_t num, unsigned int numBits);

}
