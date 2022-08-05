#pragma once

#include <stdint.h>
#include <array>

constexpr unsigned int OAM_NUM_SPRITES = 64;
constexpr uint8_t OAM_ATTRIBUTE_UNUSED_MASK = 0xE3;

struct AttributeFlags {
	uint8_t paletteIndex : 2;
	uint8_t unused : 3; // These should always read 0
	uint8_t backgroundPriority : 1;
	uint8_t flipHorizontally : 1;
	uint8_t flipVertically : 1;
};

union Attribute {
	AttributeFlags flags;
	uint8_t value;
};

struct OAMEntry {
	OAMEntry();

	uint8_t y;
	uint8_t tileID;
	Attribute attribute;
	uint8_t x;
};

struct OAMState {
	std::array<OAMEntry, OAM_NUM_SPRITES> entries;
};

class OAM {
public:
	void SoftReset();
	void HardReset();

	// used to address OAM for individual bytes (0 - 255)
	uint8_t Read(uint8_t address);
	void Write(uint8_t address, uint8_t data);

	OAMState GetState() const;
	void LoadState(OAMState& state);

	// used to address OAM for enrties (0-63)
	OAMEntry& GetEntry(unsigned int index);

private:
	std::array<OAMEntry, OAM_NUM_SPRITES> m_entries;
};
