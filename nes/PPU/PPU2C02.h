#pragma once

#include "../../Environment.h"
#include "../../utils/BitwiseUtils.h"
#include "../BusDevice.h"
#include "../CPU/CPU2A03.h"
#include "NESPicture.h"

using namespace BitwiseUtils;


constexpr uint16_t PPU_ADDRESS_RANGE_BEGIN = 0x2000;
constexpr uint16_t PPU_ADDRESS_RANGE_END = 0x3FFF;

constexpr unsigned int PPU_NUM_SCANLINES = 262;
constexpr unsigned int PPU_NUM_DOTS_PER_SCANLINE = 341;
constexpr unsigned int PPU_PRERENDER_LINE = 261;
constexpr unsigned int PPU_VISIBLE_SCANLINES_BEGIN = 0;
constexpr unsigned int PPU_VISIBLE_SCANLINES_END = NES_PICTURE_HEIGHT; // Also post-render scanline
constexpr unsigned int PPU_VISIBLE_DOT_BEGIN = 1;
constexpr unsigned int PPU_VISIBLE_DOT_END = NES_PICTURE_WIDTH + 1;

constexpr unsigned int PPU_NMI_SCANLINE = 241;
constexpr unsigned int PPU_NMI_DOT = 1;

struct PPUState {
	unsigned int frameCount;
	unsigned int scanline;
	unsigned int dot;
};

class PPU2C02 : public BusDevice {
public:
	PPU2C02(Environment* environment, CPU2A03* cpu);

	void SoftReset() override;
	void HardReset() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	PPUState GetState() const;
	void LoadState(PPUState& state);

	void Clock();

	unsigned int GetFrameCount() const;

private:
	void IncrementDotScanline();

	Environment* m_environment;

	NESPicture m_picture;

	unsigned int m_frameCount;
	unsigned int m_scanline;
	unsigned int m_dot;

	CPU2A03* m_cpu;
};