#include "NESPicture.h"

NESPixel::NESPixel() : red(0), green(0), blue(0) {}

NESPixel::NESPixel(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}

void ResetNESPicture(NESPicture& picture) {
	for (unsigned int y = 0; y < NES_PICTURE_HEIGHT; y++) {
		picture[y].fill(NESPixel());
	}
}