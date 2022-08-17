#include "PPU2C02.h"
#include "NametableDevice.h"
#include "Palette.h"

PPUState::PPUState() : backgroundShiftRegister(PPU_BACKGROUND_SHIFT_REGISTER_SIZE) {
	frameCount = 0;
	scanline = 0;
	dot = 0;
	ioLatchCounter = 0;
	ioLatchValue = 0;
	PPUCTRL.value = 0;
	PPUMASK.value = 0;
	PPUSTATUS.value = 0;
	PPUDATABuffer = 0;
	OAMADDR = 0;
	TRAMAddress.address = 0;
	VRAMAddress.address = 0;
	fineX = 0;
	loopyWriteToggle = false;
	fetchedSpriteLSB = 0;
	spriteLine.fill(SpritePixelInfo());
}


PPU2C02::PPU2C02(Environment* enviroment, CPU2A03* cpu, PaletteRAMDevice* paletteRAM) 
	: BusDevice(std::list<AddressRange>({AddressRange(PPU_ADDRESS_RANGE_BEGIN, PPU_ADDRESS_RANGE_END)})), m_environment(enviroment), m_paletteRAM(paletteRAM), m_backgroundShiftRegister(PPU_BACKGROUND_SHIFT_REGISTER_SIZE), m_secondaryOAM(&m_OAM) {
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

	m_fetchedSpriteLSB = 0;
	m_spriteLine.fill(SpritePixelInfo());

	ResetNESPicture(m_picture);
	this->setupRenderFunctions();
}

PPU2C02::~PPU2C02() {
	m_cpu = nullptr;
	m_ppuBus = nullptr;
	m_paletteRAM = nullptr;
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

	m_OAM.SoftReset();
	m_secondaryOAM.SoftReset();

	// I'm going to assume these are cleared on reset/power up
	m_nextBackgroundTileInfo = NextBackgroundTileInfo();
	m_backgroundShiftRegister.Clear();

	m_fetchedSpriteLSB = 0;
	this->ClearSpritesStack();
	m_spriteLine.fill(SpritePixelInfo());


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

	m_OAM.HardReset();
	m_secondaryOAM.HardReset();

	// I'm going to assume these are cleared on reset/power up
	m_nextBackgroundTileInfo = NextBackgroundTileInfo();
	m_backgroundShiftRegister.Clear();

	m_fetchedSpriteLSB = 0;
	this->ClearSpritesStack();
	m_spriteLine.fill(SpritePixelInfo());

	ResetNESPicture(m_picture);
}

void PPU2C02::Read(uint16_t address, uint8_t& data) {
	// This handles mirroring by squashing 0x2000 - 0x3FFF into 0x0 - 0x7
	uint8_t regSelect = ClearUpperBits16(address, 3);
	data = m_registerReadFuncs[regSelect](*this);
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

	state.oam = m_OAM.GetState();
	state.secondaryOAM = m_secondaryOAM.GetState();

	state.nextBackgroundTileInfo = m_nextBackgroundTileInfo;
	state.backgroundShiftRegister = m_backgroundShiftRegister;

	state.fetchedSpriteLSB = m_fetchedSpriteLSB;
	state.spritesToRender = m_spritesToRender;
	state.spriteLine = m_spriteLine;

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

	m_OAM.LoadState(state.oam);
	m_secondaryOAM.LoadState(state.secondaryOAM);

	m_nextBackgroundTileInfo = state.nextBackgroundTileInfo;
	m_backgroundShiftRegister = state.backgroundShiftRegister;

	m_fetchedSpriteLSB = state.fetchedSpriteLSB;
	m_spritesToRender = state.spritesToRender;
	m_spriteLine = state.spriteLine;
}

void PPU2C02::ConnectToBus(Bus* ppuBus) {
	assert(m_ppuBus == nullptr);
	m_ppuBus = ppuBus;
}

void PPU2C02::Clock() {
	for (std::function<void(PPU2C02&)> renderFunc : m_renderFuncs[m_scanline][m_dot]) {
		renderFunc(*this);
	}
	this->DecrementIOLatchCounter();
	this->IncrementDotScanline();
}


unsigned int PPU2C02::GetFrameCount() const {
	return m_frameCount;
}

