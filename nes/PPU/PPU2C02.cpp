#include "PPU2C02.h"

// TODO: Used for generating static. Remove when we do actual rendering
#include <random>


PPU2C02::PPU2C02(Environment* enviroment, CPU2A03* cpu, PaletteRAMDevice* paletteRAM, PatternTableDevice* patternTables) 
	: BusDevice(std::list<AddressRange>({AddressRange(PPU_ADDRESS_RANGE_BEGIN, PPU_ADDRESS_RANGE_END)})), m_environment(enviroment), 
	m_paletteRAM(paletteRAM), m_patternTables(patternTables) {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;
	m_cpu = cpu;
	m_ppuBus = nullptr;

	m_ioLatchValue = 0;
	m_ioLatchCounter = 0;
	m_PPUCTRL.value = 0;
	m_PPUMASK.value = 0;
	m_PPUSTATUS.value = 0;
	m_PPUDATABuffer = 0;
	m_OAMADDR = 0;

	m_TRAMAddress.address = 0;
	m_VRAMAddress.address = 0;
	m_fineX = 0;
	m_loopyWriteToggle = false;

	ResetNESPicture(m_picture);

	// TODO: Used for generating static. Remove when we do actual rendering
	srand(time(nullptr));
}

PPU2C02::~PPU2C02() {
	m_cpu = nullptr;
	m_ppuBus = nullptr;
	m_paletteRAM = nullptr;
	m_patternTables = nullptr;
}

void PPU2C02::SoftReset() {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;

	m_ioLatchValue = 0; // Not sure what the behaviour on RESET should be for the latch value
	m_ioLatchCounter = 0;
	m_PPUCTRL.SoftReset();
	m_PPUMASK.SoftReset();
	m_paletteRAM->SetGreyscaleMode(m_PPUMASK.flags.greyscaleMode);
	m_PPUSTATUS.SoftReset();
	m_PPUDATABuffer = 0;
	// OAMADDR unchanged on reset

	// On soft reset, t register and fine x are cleared but not v register
	m_TRAMAddress.address = 0;
	m_fineX = 0;

	ResetNESPicture(m_picture);
}

void PPU2C02::HardReset() {
	m_frameCount = 0;
	m_scanline = 0;
	m_dot = 0;

	m_ioLatchValue = 0;
	m_ioLatchCounter = 0;
	m_PPUCTRL.HardReset();
	m_PPUMASK.HardReset();
	m_paletteRAM->SetGreyscaleMode(m_PPUMASK.flags.greyscaleMode);
	m_PPUSTATUS.HardReset();
	m_PPUDATABuffer = 0;
	m_OAMADDR = 0;

	m_TRAMAddress.address = 0;
	m_VRAMAddress.address = 0;
	m_fineX = 0;
	m_loopyWriteToggle = false;

	ResetNESPicture(m_picture);
}

uint8_t PPU2C02::Read(uint16_t address) {
	// This handles mirroring by squashing 0x2000 - 0x3FFF into 0x0 - 0x7
	uint8_t regSelect = ClearUpperBits16(address, 3);
	return m_registerReadFuncs[regSelect](*this);
}

void PPU2C02::Write(uint16_t address, uint8_t data) {
	// This handles mirroring by squashing 0x2000 - 0x3FFF into 0x0 - 0x7
	uint8_t regSelect = ClearUpperBits16(address, 3);
	m_registerWriteFuncs[regSelect](*this, data);
}

uint8_t PPU2C02::Probe(uint16_t address) {
	return 0;
}

PPUState PPU2C02::GetState() const {
	PPUState state;
	state.frameCount = m_frameCount;
	state.scanline = m_scanline;
	state.dot = m_dot;

	state.ioLatchValue = m_ioLatchValue;
	state.ioLatchCounter = m_ioLatchCounter;
	state.PPUCTRL.value = m_PPUCTRL.value;
	state.PPUMASK.value = m_PPUMASK.value;
	state.PPUSTATUS.value = m_PPUSTATUS.value;
	state.PPUDATABuffer = m_PPUDATABuffer;
	state.OAMADDR = m_OAMADDR;

	state.TRAMAddress.address = m_TRAMAddress.address;
	state.VRAMAddress.address = m_VRAMAddress.address;
	state.fineX = m_fineX;
	state.loopyWriteToggle = m_loopyWriteToggle;

	return state;
}

void PPU2C02::LoadState(PPUState& state) {
	m_frameCount = state.frameCount;
	m_scanline = state.scanline;
	m_dot = state.dot;

	m_ioLatchValue = state.ioLatchValue;
	m_ioLatchCounter = state.ioLatchCounter;
	m_PPUCTRL.value = state.PPUCTRL.value;
	m_PPUMASK.value = state.PPUMASK.value;
	m_paletteRAM->SetGreyscaleMode(m_PPUMASK.flags.greyscaleMode);
	m_PPUSTATUS.value = state.PPUSTATUS.value;
	m_PPUDATABuffer = state.PPUDATABuffer;
	m_OAMADDR = state.OAMADDR;

	m_TRAMAddress.address = state.TRAMAddress.address;
	m_VRAMAddress.address = state.VRAMAddress.address;
	m_fineX = state.fineX;
	m_loopyWriteToggle = state.loopyWriteToggle;
}

void PPU2C02::ConnectToBus(Bus* ppuBus) {
	assert(m_ppuBus == nullptr);
	m_ppuBus = ppuBus;
}

void PPU2C02::Clock() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	static std::uniform_int_distribution<int> rng(0, 1);

	if (m_scanline >= PPU_VISIBLE_SCANLINES_BEGIN && m_scanline < PPU_VISIBLE_SCANLINES_END && m_dot >= PPU_VISIBLE_DOT_BEGIN && m_dot < PPU_VISIBLE_DOT_END) {
		int val = rng(mt);
		m_picture[m_scanline][m_dot-1].red = 255 * val;
		m_picture[m_scanline][m_dot-1].green = 255 * val;
		m_picture[m_scanline][m_dot-1].blue = 255 * val;
	}
	if (m_scanline == PPU_VISIBLE_SCANLINES_END && m_dot == 0) {
		if (m_environment != nullptr) {
			m_environment->UpdateDisplay(m_picture);
		}
	}
	
	
	if (m_scanline == PPU_NMI_SCANLINE && m_dot == PPU_NMI_DOT) {
		m_PPUSTATUS.flags.VBlank = 1;
		if (m_PPUCTRL.flags.NMIEnabled) {
			m_cpu->RaiseNMI();
		}
	}
	if (m_scanline == PPU_CLEAR_FLAGS_SCANLINE && m_dot == PPU_CLEAR_FLAGS_DOT) {
		m_PPUSTATUS.flags.spriteOverflow = 0;
		m_PPUSTATUS.flags.sprite0Hit = 0;
		m_PPUSTATUS.flags.VBlank = 0;
	}
	this->DecrementIOLatchCounter();
	this->IncrementDotScanline();
}

unsigned int PPU2C02::GetFrameCount() const {
	return m_frameCount;
}

