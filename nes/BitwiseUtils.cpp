/*
 * BitwiseUtils.cpp
 *
 *  Created on: 2 Feb 2022
 *      Author: yaniv
 *
 *     	Utility functions for 8-bit and 16-bit unsigned integers
 *     	that allow for integer overflows.
 *     	(NES had integer overflows, after all)
 */

#include "BitwiseUtils.h"
#include <assert.h>

namespace BitwiseUtils {


	uint8_t TestBit8(uint8_t num, int bit) {
		assert(bit >= 0 && bit <= 7);
		num >>= bit;
		return num & 0x1;
	}

	void SetBit8(uint8_t& target, int bit) {
		assert(bit >= 0 && bit <= 7);
		target |= (0x1 << bit);
	}

	void ClearBit8(uint8_t& target, int bit) {
		assert(bit >= 0 && bit <= 7);
		target &= ~(0x1 << bit);
	}

	void ToggleBit8(uint8_t& target, int bit) {
		assert(bit >= 0 && bit <= 7);
		target ^= (0x1 << bit);
	}

	uint8_t Add8Bit(uint8_t num1, uint8_t num2) {
		return (num1 + num2) & 0xFF;
	}

	uint8_t ShiftRight8(uint8_t num, int numBits) {
		assert(numBits >= 0 && numBits <= 8);
		return (num >> numBits) & 0xFF;
	}

	uint8_t ShiftLeft8(uint8_t num, int numBits) {
		assert(numBits >= 0 && numBits <= 8);
		return (num << numBits) & 0xFF;
	}

	void Inc8Bit(uint8_t& num) {
		num++;
		num &= 0xFF;
	}

	void Dec8Bit(uint8_t& num) {
		num--;
		num &= 0xFF;
	}


	uint8_t TestBit16(uint16_t num, int bit) {
		assert(bit >= 0 && bit <= 15);
		num >>= bit;
		return num & 0x1;
	}

	void SetBit16(uint16_t& target, int bit) {
		assert(bit >= 0 && bit <= 15);
		target |= (0x1 << bit);
	}

	void ClearBit16(uint16_t& target, int bit) {
		assert(bit >= 0 && bit <= 15);
		target &= ~(0x1 << bit);
	}

	// clear all bits up to upperBit (exclusive)
	uint16_t ClearLowerBits16(uint16_t target, int upperBit) {
		assert(upperBit >= 1 && upperBit <= 16);
		return target & (0xFFFF ^ ((0x1 << upperBit) - 1));
	}

	// clear all bits from lowerBit (inclusive)
	uint16_t ClearUpperBits16(uint16_t target, int lowerBit) {
		assert(lowerBit >= 0 && lowerBit <= 15);
		return target & ((0x1 << lowerBit)-1);
	}

	void ToggleBit16(uint16_t& target, int bit) {
		assert(bit >= 0 && bit <= 15);
		target ^= (0x1 << bit);
	}

	uint16_t Add16Bit(uint16_t num1, uint16_t num2) {
		return (num1 + num2) & 0xFFFF;
	}

	uint16_t ShiftRight16(uint16_t num, int numBits) {
		assert(numBits >= 0 && numBits <= 16);
		return (num >> numBits) & 0xFFFF;
	}

	uint16_t ShiftLeft16(uint16_t num, int numBits) {
		assert(numBits >= 0 && numBits <= 16);
		return (num << numBits) & 0xFFFF;
	}

	void Inc16Bit(uint16_t& num) {
		num++;
		num &= 0xFFFF;
	}

	void Dec16Bit(uint16_t& num) {
		num--;
		num &= 0xFFFF;
	}

	uint16_t CombineBytes(uint8_t lowByte, uint8_t highByte) {
		return lowByte | (highByte << 8);
	}

	uint8_t ExtractLowByte(uint16_t word) {
		return word & 0xFF;
	}

	uint8_t ExtractHighByte(uint16_t word) {
		return (word & 0xFF00) >> 8;
	}

	bool IsUpperByteTheSame(uint16_t word1, uint16_t word2) {
		return ExtractHighByte(word1) == ExtractHighByte(word2);
	}

}


