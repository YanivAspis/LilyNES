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


	uint8_t TestBit8(uint8_t num, unsigned int bit) {
		assert(bit >= 0 && bit <= 7);
		num >>= bit;
		return num & 0x1;
	}

	void SetBit8(uint8_t& target, unsigned int bit) {
		assert(bit >= 0 && bit <= 7);
		target |= (0x1 << bit);
	}

	void ClearBit8(uint8_t& target, unsigned int bit) {
		assert(bit >= 0 && bit <= 7);
		target &= ~(0x1 << bit);
	}

	void ToggleBit8(uint8_t& target, unsigned int bit) {
		assert(bit >= 0 && bit <= 7);
		target ^= (0x1 << bit);
	}

	// clear all bits up to upperBit (exclusive)
	uint8_t ClearLowerBits8(uint8_t target, unsigned int upperBit) {
		assert(upperBit >= 1 && upperBit <= 8);
		return target & (0xFF ^ ((0x1 << upperBit) - 1));
	}

	// clear all bits from lowerBit (inclusive)
	uint8_t ClearUpperBits8(uint8_t target, unsigned int lowerBit) {
		assert(lowerBit >= 0 && lowerBit <= 7);
		return target & ((0x1 << lowerBit) - 1);
	}

	uint8_t Add8Bit(uint8_t num1, uint8_t num2) {
		return (num1 + num2) & 0xFF;
	}

	uint8_t ShiftRight8(uint8_t num, unsigned int numBits) {
		assert(numBits >= 0 && numBits <= 8);
		return (num >> numBits) & 0xFF;
	}

	uint8_t ShiftLeft8(uint8_t num, unsigned int numBits) {
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


	uint8_t TestBit16(uint16_t num, unsigned int bit) {
		assert(bit >= 0 && bit <= 15);
		num >>= bit;
		return num & 0x1;
	}

	void SetBit16(uint16_t& target, unsigned int bit) {
		assert(bit >= 0 && bit <= 15);
		target |= (0x1 << bit);
	}

	void ClearBit16(uint16_t& target, unsigned int bit) {
		assert(bit >= 0 && bit <= 15);
		target &= ~(0x1 << bit);
	}

	// clear all bits up to upperBit (exclusive)
	uint16_t ClearLowerBits16(uint16_t target, unsigned int upperBit) {
		assert(upperBit >= 1 && upperBit <= 16);
		return target & (0xFFFF ^ ((0x1 << upperBit) - 1));
	}

	// clear all bits from lowerBit (inclusive)
	uint16_t ClearUpperBits16(uint16_t target, unsigned int lowerBit) {
		assert(lowerBit >= 0 && lowerBit <= 15);
		return target & ((0x1 << lowerBit) - 1);
	}

	void ToggleBit16(uint16_t& target, unsigned int bit) {
		assert(bit >= 0 && bit <= 15);
		target ^= (0x1 << bit);
	}

	uint16_t Add16Bit(uint16_t num1, uint16_t num2) {
		return (num1 + num2) & 0xFFFF;
	}

	uint16_t ShiftRight16(uint16_t num, unsigned int numBits) {
		assert(numBits >= 0 && numBits <= 16);
		return (num >> numBits) & 0xFFFF;
	}

	uint16_t ShiftLeft16(uint16_t num, unsigned int numBits) {
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

	uint16_t AddRelativeAddress(uint16_t baseAddress, uint8_t signedRelativeAddress) {
		uint16_t unsignedRelativeAddress;

		// Handle sign
		if (TestBit8(signedRelativeAddress, 7)) {
			unsignedRelativeAddress = CombineBytes(signedRelativeAddress, 0xFF);
		}
		else {
			unsignedRelativeAddress = signedRelativeAddress;
		}

		return Add16Bit(baseAddress, unsignedRelativeAddress);
	}

	bool IsUpperByteTheSame(uint16_t word1, uint16_t word2) {
		return ExtractHighByte(word1) == ExtractHighByte(word2);
	}

	uint32_t Add32Bit(uint32_t num1, uint32_t num2) {
		return (num1 + num2) & 0xFFFFFFFF;
	}

	// Little endian inputs (a LSB, d MSB)
	uint32_t CombineBytes32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
		return a | b << 8 | c << 16 | d << 24;
	}

	std::array<uint8_t, 4> SplitBytes32(uint32_t num) {
		std::array<uint8_t, 4> result;
		result[0] = num & 0xFF;
		result[1] = (num & 0xFF00) >> 8;
		result[2] = (num & 0xFF0000) >> 16;
		result[3] = (num & 0xFF000000) >> 24;
		return result;
	}

	uint32_t RotateLeft32Bit(uint32_t num, unsigned int numBits) {
		assert(numBits >= 0 && numBits <= 32);
		uint64_t expandedNum = num;
		expandedNum <<= numBits;
		uint64_t remainder = expandedNum & 0xFFFFFFFF00000000;
		uint32_t result = (expandedNum & 0xFFFFFFFF) | (remainder >> 32);
		return result;
	}

}


