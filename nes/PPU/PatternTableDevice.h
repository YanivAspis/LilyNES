#pragma once

#include <array>
#include "../BusDevice.h"
#include "../mappers/Cartridge.h"


constexpr uint16_t PATTERN_TABLE_BEGIN_ADDRESS = 0x0;
constexpr uint16_t PATTERN_TABLE_END_ADDRESS = 0x1FFF;
constexpr uint16_t PATTERN_TABLE_SIZE = 0x1000;

constexpr unsigned int PATTERN_TABLE_NUM_TILES_WIDTH = 16;
constexpr unsigned int PATTERN_TABLE_NUM_TILES_HEIGHT = PATTERN_TABLE_NUM_TILES_WIDTH;
constexpr unsigned int PATTERN_TABLE_TILE_WIDTH = 8;
constexpr unsigned int PATTERN_TABLE_TILE_HEIGHT = PATTERN_TABLE_TILE_WIDTH;
constexpr unsigned int PATTERN_TABLE_MSB_OFFSET = PATTERN_TABLE_TILE_HEIGHT;

struct PatternTableState {
	std::array<uint8_t, PATTERN_TABLE_SIZE> patternTable0;
	std::array<uint8_t, PATTERN_TABLE_SIZE> patternTable1;
};

class PatternTableDevice : public BusDevice {
public:
	PatternTableDevice();
	~PatternTableDevice();

	void SoftReset() override;
	void HardReset() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	PatternTableState GetState() const;
	void LoadState(PatternTableState& state);

	void ConnectCartridge(Cartridge* cartridge);
	void DisconnectCartridge();

	static uint16_t GetTileLowBitsAddress(unsigned int patternTable, uint8_t tileID, uint8_t row);
	static uint16_t GetTileHighBitsAddress(unsigned int patternTable, uint8_t tileID, uint8_t row);
	static std::array<uint8_t, PATTERN_TABLE_TILE_WIDTH> GetRowColourIndices(uint8_t rowLowBits, uint8_t rowHighBits);

private:
	Cartridge* m_cartridge;
};
