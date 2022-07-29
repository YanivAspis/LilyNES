#pragma once

#include <array>
#include "../BusDevice.h"
#include "../mappers/Cartridge.h"

constexpr unsigned int NAMETABLE_BEGIN_ADDRESS = 0x2000;
constexpr unsigned int NAMETABLE_END_ADDRESS = 0x3EFF;

constexpr unsigned int NAMETABLE_NUM_X_TILES = 32;
constexpr unsigned int NAMETABLE_NUM_Y_TILES = 30;
constexpr unsigned int NAMETABLE_COARSE_Y_MAX = 31;
constexpr unsigned int NAMETABLE_TILE_WIDTH = 8;
constexpr unsigned int NAMETABLE_TILE_HEIGHT = NAMETABLE_TILE_WIDTH;
constexpr unsigned int NAMETABLE_SIZE = NAMETABLE_NUM_X_TILES * (NAMETABLE_COARSE_Y_MAX + 1);

struct NametableState {
	std::array<uint8_t, NAMETABLE_SIZE> nametable0;
	std::array<uint8_t, NAMETABLE_SIZE> nametable1;
};

class NametableDevice : public BusDevice {
public:
	NametableDevice();
	~NametableDevice();

	void SoftReset() override;
	void HardReset() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	PatternTableState GetState() const;
	void LoadState(PatternTableState& state);

	void ConnectCartridge(Cartridge* cartridge);
	void DisconnectCartridge();

private:
	// This function will take care of nametable mirroring and indexing by returning a pointer to the correct data address
	uint8_t* GetDataPointer(uint16_t address);

	std::array<uint8_t, NAMETABLE_SIZE>* GetNametablePointerHorizontalMirroring(uint16_t address);
	std::array<uint8_t, NAMETABLE_SIZE>* GetNametablePointerVerticalMirroring(uint16_t address);
	std::array<uint8_t, NAMETABLE_SIZE>* GetNametablePointerSingleScreenMirroring(uint16_t address);

	std::array<uint8_t, NAMETABLE_SIZE> m_nametable0;
	std::array<uint8_t, NAMETABLE_SIZE> m_nametable1;

	Cartridge* m_cartridge;

	std::map<MirroringMode, std::function<NametableDevice(std::array<uint8_t, NAMETABLE_SIZE>*, uint16_t)>> m_mirroringFunctions;
};
