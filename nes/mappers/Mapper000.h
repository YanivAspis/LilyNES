#pragma once

#include "Cartridge.h"
#include "../ROM/INESFile.h"

struct Mapper000State : public MapperAdditionalState {};

class Mapper000 : public Cartridge {
public:
	Mapper000(const INESFile& romFile);

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	uint8_t PPURead(uint16_t address) override;
	void PPUWrite(uint16_t address, uint8_t data) override;

	MirroringMode GetCurrentMirroringMode() override;

private:
	void GetAdditionalState(std::shared_ptr<MapperAdditionalState> state) const;
	void LoadAdditionalState(std::shared_ptr<MapperAdditionalState> state);

	uint8_t PRGROMRead(uint16_t address);
	void PRGROMWrite(uint16_t address, uint8_t data);
	uint8_t PRGRAMRead(uint16_t address);
	void PRGRAMWrite(uint16_t address, uint8_t data);

	MirroringMode m_mirroringMode;
	size_t m_numPRGROMBanks;
};
