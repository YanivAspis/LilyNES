#pragma once

#include <array>
#include "NESPicture.h"

constexpr unsigned int PALETTE_NUM_SUPPORTED_COLOURS = 64;

// For now, I'll use a static table without considering emphasis. I'll change this later to dynamically loading a pal file and handling emphasis

static const std::array<NESPixel, PALETTE_NUM_SUPPORTED_COLOURS> BASE_PALETTE_TO_COLOUR_TABLE = {
	NESPixel(84, 84, 84),     // 0x00
	NESPixel(0, 30, 116),     // 0x01
	NESPixel(8, 16, 144),     // 0x02
	NESPixel(48, 0, 136),     // 0x03
	NESPixel(68, 0, 100),     // 0x04
	NESPixel(92, 0, 48),      // 0x05
	NESPixel(84, 4, 0),       // 0x06
	NESPixel(60, 24, 0),      // 0x07
	NESPixel(32, 42, 0),      // 0x08
	NESPixel(8, 58, 0),       // 0x09
	NESPixel(0, 64, 0),       // 0x0A
	NESPixel(0, 60, 0),        // 0x0B
	NESPixel(0, 50, 60),      // 0x0C
	NESPixel(0, 0, 0),        // 0x0D
	NESPixel(0, 0, 0),        // 0x0E
	NESPixel(0, 0, 0),        // 0x0F

	NESPixel(152, 150, 152),  // 0x10
	NESPixel(8, 76, 196),     // 0x11
	NESPixel(48, 50, 236),    // 0x12
	NESPixel(92, 30, 228),    // 0x13
	NESPixel(136, 20, 176),   // 0x14
	NESPixel(160, 20, 100),   // 0x15
	NESPixel(152, 34, 32),    // 0x16
	NESPixel(120, 60, 0),     // 0x17
	NESPixel(84, 90, 0),      // 0x18
	NESPixel(40, 114, 0),     // 0x19
	NESPixel(8, 124, 0),      // 0x1A
	NESPixel(0, 118, 40),     // 0x1B
	NESPixel(0, 102, 120),    // 0x1C
	NESPixel(0, 0, 0),        // 0x1D
	NESPixel(0, 0, 0),        // 0x1E
	NESPixel(0, 0, 0),        // 0x1F

	NESPixel(236, 238, 236),  // 0x20
	NESPixel(76, 154, 236),   // 0x21
	NESPixel(120, 124, 236),  // 0x22
	NESPixel(176, 98, 236),   // 0x23
	NESPixel(228, 84, 236),   // 0x24
	NESPixel(236, 88, 180),   // 0x25
	NESPixel(236, 106, 100),  // 0x26
	NESPixel(212, 136, 32),   // 0x27
	NESPixel(160, 170, 0),    // 0x28
	NESPixel(116, 196, 0),    // 0x29
	NESPixel(76, 208, 32),    // 0x2A
	NESPixel(56, 204, 108),   // 0x2B
	NESPixel(56, 180, 204),   // 0x2C
	NESPixel(60, 60, 60),     // 0x2D
	NESPixel(0, 0, 0),        // 0x2E
	NESPixel(0, 0, 0),        // 0x2F

	NESPixel(236, 238, 236),  // 0x30
	NESPixel(168, 204, 236),  // 0x31
	NESPixel(188, 188, 236),  // 0x32
	NESPixel(212, 178, 236),  // 0x33
	NESPixel(236, 174, 236),  // 0x34
	NESPixel(236, 174, 212),  // 0x35
	NESPixel(236, 180, 176),  // 0x36
	NESPixel(228, 196, 144),  // 0x37
	NESPixel(204, 210, 120),  // 0x38
	NESPixel(180, 222, 120),  // 0x39
	NESPixel(168, 226, 144),  // 0x3A
	NESPixel(152, 226, 180),  // 0x3B
	NESPixel(160, 214, 228),  // 0x3C
	NESPixel(160, 162, 160),  // 0x3D
	NESPixel(0, 0, 0),        // 0x3E
	NESPixel(0, 0, 0)         // 0x3F
};

NESPixel GetColourFromPalette(uint8_t paletteEntry, unsigned int emphasizeRed, unsigned int emphasizeGreen, unsigned int emphasizeBlue);
