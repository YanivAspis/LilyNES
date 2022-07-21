#pragma once

#include <stdint.h>
#include <array>

constexpr unsigned int NES_PICTURE_WIDTH = 256;
constexpr unsigned int NES_PICTURE_HEIGHT = 240;

struct NESPixel {
	NESPixel();
	NESPixel(uint8_t r, uint8_t g, uint8_t b);

	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

typedef std::array<std::array<NESPixel, NES_PICTURE_WIDTH>, NES_PICTURE_HEIGHT> NESPicture;

void ResetNESPicture(NESPicture& picture);
