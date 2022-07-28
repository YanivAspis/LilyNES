#include "Palette.h"

NESPixel GetColourFromPalette(uint8_t paletteEntry, unsigned int emphasizeRed, unsigned int emphasizeGreen, unsigned int emphasizeBlue) {
	return BASE_PALETTE_TO_COLOUR_TABLE[paletteEntry];
}