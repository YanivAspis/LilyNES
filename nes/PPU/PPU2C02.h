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



struct PPUCTRLFlags {
	uint8_t NametableX : 1;
	uint8_t NametableY : 1;
	uint8_t IncrementMode : 1;             
	uint8_t SpritePatternTable : 1;       // Sprite pattern table select (ignored in 8x16 mode)
	uint8_t BackgroundPatternTable : 1;   // Background pattern table select
	uint8_t SpriteSize : 1;               // Sprite size (8x8 or 8x16)
	uint8_t PPUMaster : 1;                // PPU master/slave - Not sure what this is meant to do. I will ignore this
	uint8_t NMIEnabled : 1;               // Generate NMI at vertical blank (apparently not only at the start, but any time in vertical blank if this is set)
};

union PPUCTRLRegister {
	PPUCTRLFlags flags;
	uint8_t value;

	void SoftReset();
	void HardReset();
};


struct PPUMASKFlags {
	uint8_t GreyscaleMode : 1;           // Renders only in greyscale colours
	uint8_t ShowLeftmostBackground : 1;  // Show background on leftmost 8 pixels of screen
	uint8_t ShowLeftmostSprites : 1;     // Show sprites on leftmost 8 pixels of sscreen
	uint8_t RenderBackground : 1;        // Render background
	uint8_t RenderSprites : 1;           // Render sprites
	uint8_t EmphasizeRed : 1;            // Has to do with composite video signals - I'll ignore this
	uint8_t EmphasizeGreen : 1;          // Has to do with composite video signals - I'll ignore this
	uint8_t Emphasizeblue : 1;           // Has to do with composite video signals - I'll ignore this
};

union PPUMASKRegister {
	PPUMASKFlags flags;
	uint8_t value;

	void SoftReset();
	void HardReset();
};


struct PPUSTATUSFlags {
	uint8_t Unused : 5;          // Should have open bus behaviour
	uint8_t SpriteOverflow : 1;  // Set to 1 if more than 8 sprites on scanline (buggy on NES though)
	uint8_t Sprite0Hit : 1;      // Set to 1 when an opaque pixel of sprite 0 is rendered on an opaque pixel of background
	uint8_t VBlank : 1;          // Set to 1 when entering VBlank (sort of). Cleared when read
};

union PPUSTATUSRegister {
	PPUSTATUSFlags flags;
	uint8_t value;

	void SoftReset();
	void HardReset();
};



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

	// Register read/write functions
	uint8_t PPUCTRLRead();
	void PPUCTRLWrite(uint8_t data);

	uint8_t PPUMASKRead();
	void PPUMASKWrite(uint8_t data);

	uint8_t PPUSTATUSRead();
	void PPUSTATUSWrite(uint8_t data);

	uint8_t OAMADDRRead();
	void OAMADDRWrite(uint8_t data);

	uint8_t OAMDATARead();
	void OAMDATAWrite(uint8_t data);

	uint8_t PPUSCROLLRead();
	void PPUSCROLLWrite(uint8_t data);

	uint8_t PPUADDRRead();
	void PPUADDRWrite(uint8_t data);

	uint8_t PPUDATARead();
	void PPUDATAWrite(uint8_t data);


	void IncrementDotScanline();

	Environment* m_environment;

	NESPicture m_picture;

	unsigned int m_frameCount;
	unsigned int m_scanline;
	unsigned int m_dot;

	CPU2A03* m_cpu;

	// "Open Bus behaviour": PPU has an internal latch that gets filled during CPU reads/writes. Reading from write-only registers should return this
	// TODO: Add decay behaviour
	uint8_t m_latchValue;
	PPUCTRLRegister m_PPUCTRL;
	PPUMASKRegister m_PPUMASK;
	PPUSTATUSRegister m_PPUSTATUS;

	const std::array<std::function<uint8_t(PPU2C02&)>, 8> m_registerReadFuncs = {
		&PPU2C02::PPUCTRLRead,
		&PPU2C02::PPUMASKRead,
		&PPU2C02::PPUSTATUSRead,
		&PPU2C02::OAMADDRRead,
		&PPU2C02::OAMDATARead,
		&PPU2C02::PPUSCROLLRead,
		&PPU2C02::PPUADDRRead,
		&PPU2C02::PPUDATARead
	};
	const std::array<std::function<void(PPU2C02&, uint8_t)>, 8> m_registerWriteFuncs = {
		&PPU2C02::PPUCTRLWrite,
		&PPU2C02::PPUMASKWrite,
		&PPU2C02::PPUSTATUSWrite,
		&PPU2C02::OAMADDRWrite,
		&PPU2C02::OAMDATAWrite,
		&PPU2C02::PPUSCROLLWrite,
		&PPU2C02::PPUADDRWrite,
		&PPU2C02::PPUDATAWrite
	};
};