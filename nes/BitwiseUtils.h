#pragma once

#include <cstdint>


namespace BitwiseUtils {

uint8_t TestBit8(uint8_t num, int bit);
void SetBit8(uint8_t& target, int bit);
void ClearBit8(uint8_t& target, int bit);
void ToggleBit8(uint8_t& target, int bit);
uint8_t Add8Bit(uint8_t num1, uint8_t num2);
uint8_t ShiftRight8(uint8_t num, int numBits);
uint8_t ShiftLeft8(uint8_t num, int numBits);
void Inc8Bit(uint8_t& num);
void Dec8Bit(uint8_t& num);

uint8_t TestBit16(uint16_t num, int bit);
void SetBit16(uint16_t& target, int bit);
void ClearBit16(uint16_t& target, int bit);
void ToggleBit16(uint16_t& target, int bit);
uint16_t ClearLowerBits16(uint16_t target, int upperBit);
uint16_t ClearUpperBits16(uint16_t target, int lowerBit);
uint16_t Add16Bit(uint16_t num1, uint16_t num2);
uint16_t ShiftRight16(uint16_t num, int numBits);
uint16_t ShiftLeft16(uint16_t num, int numBits);
void Inc16Bit(uint16_t& num);
void Dec16Bit(uint16_t& num);
uint16_t CombineBytes(uint8_t lowByte, uint8_t highByte);
uint8_t ExtractLowByte(uint16_t word);
uint8_t ExtractHighByte(uint16_t word);

bool IsUpperByteTheSame(uint16_t word1, uint16_t word2);

}
