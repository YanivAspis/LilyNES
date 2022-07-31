#pragma once

#include <array>

#include "../BusDevice.h"

constexpr uint16_t PALETTE_RAM_BEGIN_ADDRESS = 0x3F00;
constexpr uint16_t PALETTE_RAM_END_ADDRESS = 0x3FFF;
constexpr uint8_t PALETTE_SPRITE_MEMORY_OFFSET = 0x10;

constexpr unsigned int PALETTE_NUM_PALETTE_COLOURS = 3;
constexpr unsigned int PALETTE_NUM_BACKGROUND_PALETTES = 4;
constexpr unsigned int PALETTE_NUM_SPRITE_PALETTES = 4;
constexpr uint8_t PALETTE_GREY_MODIFIER = 0x30;

struct PaletteRAMState {
	PaletteRAMState();
	uint8_t universalBackgroundColour;
	std::array<uint8_t, PALETTE_NUM_BACKGROUND_PALETTES - 1> otherBackgroundColours; // Other background colours can be written to, but are not normally displayed
	std::array<std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>, PALETTE_NUM_BACKGROUND_PALETTES> backgroundPalettes;
	std::array<std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>, PALETTE_NUM_SPRITE_PALETTES> spritePalettes;
	bool greyscaleMode;
};

class PaletteRAMDevice : public BusDevice {
public:
	PaletteRAMDevice();

	virtual void SoftReset() override;
	virtual void HardReset() override;

	virtual uint8_t Read(uint16_t address) override;
	virtual void Write(uint16_t address, uint8_t data)  override;

	virtual uint8_t Probe(uint16_t address) override;

	PaletteRAMState GetState() const;
	void LoadState(PaletteRAMState& state);

	void SetGreyscaleMode(bool greyscaleMode);
	uint16_t GetBackgroundPaletteAddress(uint8_t palette, uint8_t index);
	uint16_t GetSpritePaletteAddress(uint8_t palette, uint8_t index);

private:
	uint8_t* GetColourPointer(uint16_t address, bool forWrite);

	uint8_t m_universalBackgroundColour;
	std::array<uint8_t, PALETTE_NUM_BACKGROUND_PALETTES - 1> m_otherBackgroundColours;
	std::array<std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>, PALETTE_NUM_BACKGROUND_PALETTES> m_backgroundPalettes;
	std::array<std::array<uint8_t, PALETTE_NUM_PALETTE_COLOURS>, PALETTE_NUM_SPRITE_PALETTES> m_spritePalettes;
	bool m_greyscaleMode;
};