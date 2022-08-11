#pragma once

#include <vector>
#include <map>
#include <any>

#include "../ROM/INESFile.h"
#include "../BusDevice.h"
#include "../../utils/NESUtils.h"

using namespace NESUtils;

constexpr uint16_t PRG_RAM_START_ADDRESS = 0x6000;
constexpr uint16_t PRG_RAM_END_ADDRESS = 0x7FFF;
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

struct LogicalBank {
	LogicalBank(uint16_t sAddress, uint16_t bankSize);
	uint16_t startAddress;
	uint16_t size;
};

// Each mapper inherits this to define its own specific state information
struct MapperAdditionalState {
	virtual ~MapperAdditionalState() = 0;
};

struct CartridgeState {
	CartridgeState();

	uint8_t mapperID;

	std::vector<uint8_t> CHRROM;
	std::vector<uint8_t> PRGRAM;

	std::vector<LogicalBank> PRGLogicalBanks;
	std::vector<LogicalBank> CHRLogicalBanks;
	std::map<unsigned int, size_t> PRGBankMapping;
	std::map<unsigned int, size_t> CHRBankMapping;


	// Additional Mapper-specific state information
	std::any additionalState;
};

class Cartridge : public BusDevice {
public:
	Cartridge(const INESFile& romFile);

	virtual void SoftReset() override;
	virtual void HardReset() override;

	virtual void SetupLogicalBanks() = 0;
	virtual void InitializeBankMapping() = 0;

	virtual uint8_t Read(uint16_t address) override;
	virtual void Write(uint16_t address, uint8_t data) override;

	virtual uint8_t PPURead(uint16_t address);
	virtual void PPUWrite(uint16_t address, uint8_t data);

	virtual uint8_t Probe(uint16_t address);
	virtual uint8_t ProbePPU(uint16_t address);

	virtual MirroringMode GetCurrentMirroringMode() = 0;

	CartridgeState GetState() const;
	void LoadState(CartridgeState& state);

protected:
	virtual uint8_t PRGROMRead(uint16_t address);
	virtual void PRGROMWrite(uint16_t address, uint8_t data) = 0;
	virtual uint8_t PRGRAMRead(uint16_t address);
	virtual void PRGRAMWrite(uint16_t address, uint8_t data);
	virtual uint8_t CHRROMRead(uint16_t address);
	virtual void CHRROMWrite(uint16_t address, uint8_t data);

	std::vector<uint8_t> m_PRGROM;

	// CHRROM can act as CHRRAM depending on m_CHRRAMEnabled flag
	std::vector<uint8_t> m_CHRROM;

	std::vector<uint8_t> m_PRGRAM;

	bool m_CHRRAMEnabled;

	std::vector<LogicalBank> m_PRGLogicalBanks;
	std::vector<LogicalBank> m_CHRLogicalBanks;
	std::map<unsigned int, size_t> m_PRGBankMapping;
	std::map<unsigned int, size_t> m_CHRBankMapping;

private:
	// Each mapper implements these to save/load its additional state
	virtual std::any GetAdditionalState() const = 0;
	virtual void LoadAdditionalState(std::any state) = 0;

	unsigned int GetLogicalBankIndex(std::vector<LogicalBank> logicalBanks, uint16_t address);

	uint8_t m_mapperID;
};
