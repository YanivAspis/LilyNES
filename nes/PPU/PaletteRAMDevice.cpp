#include "PaletteRAMDevice.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

PaletteRAMState::PaletteRAMState() {
	universalBackgroundColour = 0;
	otherBackgroundColours.fill(0);
	for (std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>& palette : backgroundPalettes) {
		palette.fill(0);
	}
	for (std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>& palette : spritePalettes) {
		palette.fill(0);
	}
	greyscaleMode = false;
}

PaletteRAMDevice::PaletteRAMDevice() : BusDevice(std::list({AddressRange(PALETTE_RAM_BEGIN_ADDRESS, PALETTE_RAM_END_ADDRESS)})) {
	m_universalBackgroundColour = 0;
	m_otherBackgroundColours.fill(0);
	for (std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>& palette : m_backgroundPalettes) {
		palette.fill(0);
	}
	for (std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>& palette : m_spritePalettes) {
		palette.fill(0);
	}
	m_greyscaleMode = false;
}

// Palette RAM unchanged on reset
void PaletteRAMDevice::SoftReset() {}

// On power-up, there is no specified Palette RAM behaviour, so I just set it all to 0
void PaletteRAMDevice::HardReset() {
	m_universalBackgroundColour = 0;
	for (std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>& palette : m_backgroundPalettes) {
		palette.fill(0);
	}
	for (std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>& palette : m_spritePalettes) {
		palette.fill(0);
	}
}

void PaletteRAMDevice::Read(uint16_t address, uint8_t& data) {
	data = *(this->GetColourPointer(address, false));
	if (m_greyscaleMode) {
		// Change colour to grey
		data &= PALETTE_GREY_MODIFIER;
	}
	// Top two bits should always be 0
	data = ClearUpperBits8(data, 6);
}

void PaletteRAMDevice::Write(uint16_t address, uint8_t data) {
	// Top two bits should always be 0
	*(this->GetColourPointer(address, true)) = ClearUpperBits8(data, 6);
}

uint8_t PaletteRAMDevice::Probe(uint16_t address) {
	uint8_t data = 0;
	this->Read(address, data);
	return data;
}

PaletteRAMState PaletteRAMDevice::GetState() const {
	PaletteRAMState state;
	state.universalBackgroundColour = m_universalBackgroundColour;
	state.otherBackgroundColours = m_otherBackgroundColours;
	state.backgroundPalettes = m_backgroundPalettes;
	state.spritePalettes = m_spritePalettes;
	state.greyscaleMode = m_greyscaleMode;
	return state;
}

void PaletteRAMDevice::LoadState(PaletteRAMState& state) {
	m_universalBackgroundColour = state.universalBackgroundColour;
	m_otherBackgroundColours = state.otherBackgroundColours;
	m_backgroundPalettes = state.backgroundPalettes;
	m_spritePalettes = state.spritePalettes;
	m_greyscaleMode = state.greyscaleMode;
}

void PaletteRAMDevice::SetGreyscaleMode(bool greyscaleMode) {
	m_greyscaleMode = greyscaleMode;
}

uint16_t PaletteRAMDevice::GetBackgroundPaletteAddress(uint8_t palette, uint8_t index) {
	return PALETTE_RAM_BEGIN_ADDRESS + palette * (PALETTE_NUM_PALETTE_COLOURS + 1) + index;
}

uint16_t PaletteRAMDevice::GetSpritePaletteAddress(uint8_t palette, uint8_t index) {
	return PALETTE_RAM_BEGIN_ADDRESS + PALETTE_NUM_BACKGROUND_PALETTES * (PALETTE_NUM_PALETTE_COLOURS + 1) + palette * (PALETTE_NUM_PALETTE_COLOURS + 1) + index;
}

uint8_t* PaletteRAMDevice::GetColourPointer(uint16_t address, bool forWrite) {
	// First mirror to 0x00 - 0x1F
	address = ClearUpperBits16(address, 5);

	// All addresses divisible by four are mapped to the universal background colour
	// TODO: 0x3F04, 0x3F08, 0x3F0C can be written to, but do not normally display. Emulate this
	if (address % (PALETTE_NUM_PALETTE_COLOURS + 1) == 0) {
		if (forWrite && address % (PALETTE_NUM_BACKGROUND_PALETTES * (PALETTE_NUM_PALETTE_COLOURS + 1)) != 0) {
			// Return dummy background palletes
			unsigned int colourIndex = (address % (PALETTE_NUM_BACKGROUND_PALETTES * (PALETTE_NUM_PALETTE_COLOURS + 1)) / PALETTE_NUM_BACKGROUND_PALETTES) - 1;
			return &m_otherBackgroundColours[colourIndex];
		}
		else {
			return &m_universalBackgroundColour;
		}
	}
	else if (address < PALETTE_SPRITE_MEMORY_OFFSET) {
		// background colour
		unsigned int paletteIndex = address / PALETTE_NUM_BACKGROUND_PALETTES;
		unsigned int colourIndex = address % (PALETTE_NUM_PALETTE_COLOURS + 1) - 1;
		return &m_backgroundPalettes[paletteIndex][colourIndex];
	}
	else {
		// sprite colour
		address -= PALETTE_SPRITE_MEMORY_OFFSET;
		unsigned int paletteIndex = address / PALETTE_NUM_SPRITE_PALETTES;
		unsigned int colourIndex = address % (PALETTE_NUM_PALETTE_COLOURS + 1) - 1;
		return &m_spritePalettes[paletteIndex][colourIndex];
	}
}