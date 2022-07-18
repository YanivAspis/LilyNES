#pragma once

#include <vector>

#include "../ROM/INESFile.h"
#include "../BusDevice.h"
#include "../../utils/NESUtils.h"

using namespace NESUtils;

constexpr uint16_t PRG_RAM_START_ADDRESS = 0x6000;
constexpr uint16_t PRG_RAM_END_ADDRESS = 0x7999;
constexpr uint16_t PRG_ROM_START_ADDRESS = 0x8000;
constexpr uint16_t PRG_ROM_END_ADDRESS = 0xFFFF;


class UnsupportedMapperException : public std::exception {
public:
	UnsupportedMapperException(uint8_t mapperID) 
	{
		std::string mapperStr = DecUint8ToString(mapperID);
		m_exceptMessage = "Unsupported mapper: " + mapperStr;
	}

	const char* what() const throw () {
		return m_exceptMessage.c_str();
	}

private:
	std::string m_exceptMessage;
};


enum MirroringMode {
	MIRRORING_HORIZONTAL,
	MIRRORING_VERTICAL,
	MIRRORING_SINGLE_SCREEN,

	// Modes I'm not going to implement currently...
	MIRRORING_FOUR_SCREEN,
	MIRRORING_DIAGONAL,
	MIRRORING_L_SHAPED // Needed for Castlevania 3
};

// Each mapper inherits this to define its own specific state information
struct MapperAdditionalState {
	virtual ~MapperAdditionalState() = 0;
};

struct CartridgeState {
	CartridgeState();

	std::vector<uint8_t> CHRROM;
	std::vector<uint8_t> PRGRAM;

	// Additional Mapper-specific state information
	std::shared_ptr<MapperAdditionalState> additionalState;
};

class Cartridge : public BusDevice {
public:
	Cartridge(const INESFile& romFile);

	void SoftReset() override;
	void HardReset() override;

	virtual uint8_t PPURead(uint16_t address) = 0;
	virtual void PPUWrite(uint16_t address, uint8_t data) = 0;

	virtual MirroringMode GetCurrentMirroringMode() = 0;

	CartridgeState GetState() const;
	void LoadState(CartridgeState& state);

protected:
	std::vector<uint8_t> m_PRGROM;

	// CHRROM can act as CHRRAM depending on m_CHRRAMEnabled flag
	std::vector<uint8_t> m_CHRROM;

	std::vector<uint8_t> m_PRGRAM;

	bool m_CHRRAMEnabled;

private:
	// Each mapper implements these to save/load its additional state
	virtual void GetAdditionalState(std::shared_ptr<MapperAdditionalState> state) const = 0;
	virtual void LoadAdditionalState(std::shared_ptr<MapperAdditionalState> state) = 0;
};
