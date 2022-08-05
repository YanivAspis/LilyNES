#include "PPU2C02.h"
#include "Nametabledevice.h"
#include "Palette.h"

NextBackgroundTileInfo::NextBackgroundTileInfo() {
	tileID = 0;
	paletteIndex = 0;
	coloursLSB = 0;
	coloursMSB = 0;
}

PixelColourInfo::PixelColourInfo() {
	paletteIndex = 0;
	colourIndex = 0;
}

void PPU2C02::IncrementDotScanline() {
	m_dot++;
	if (m_dot == PPU_NUM_DOTS_PER_SCANLINE) {
		m_dot = 0;
		m_scanline++;
	}
	if (m_scanline == PPU_NUM_SCANLINES) {
		m_scanline = 0;
		m_frameCount++;

		// Skip (0,0) cycle on odd frames
		if (m_frameCount % 2 == 1) {
			m_dot++;
		}
	}
}

void PPU2C02::setupRenderFunctions() {
	// Add all pixel rendering functions
	for (unsigned int scanline = PPU_VISIBLE_SCANLINES_BEGIN; scanline < PPU_VISIBLE_SCANLINES_END; scanline++) {
		for (unsigned int dot = PPU_VISIBLE_DOT_BEGIN; dot < PPU_VISIBLE_DOT_END; dot++) {
			m_renderFuncs[scanline][dot].push_back(&PPU2C02::RenderPixel);
		}
	}

	// Fetching background graphics data
	this->setupFetchFunctionTiming(PPU_NAMETABLE_TILE_ID_FETCH_BEGIN, PPU_NAMETABLE_TILE_ID_FETCH_END, std::vector<unsigned int>(PPU_NAMETABLE_TILE_ID_FETCH_EXTRA.begin(), PPU_NAMETABLE_TILE_ID_FETCH_EXTRA.end()), &PPU2C02::FetchTileIDFromNametable);
	this->setupFetchFunctionTiming(PPU_NAMETABLE_ATTRIBUTE_FETCH_BEGIN, PPU_NAMETABLE_ATTRIBUTE_FETCH_END, std::vector<unsigned int>(PPU_NAMETABLE_ATTRIBUTE_FETCH_EXTRA.begin(), PPU_NAMETABLE_ATTRIBUTE_FETCH_EXTRA.end()), &PPU2C02::FetchPaletteIndexFromNametable);
	this->setupFetchFunctionTiming(PPU_BACKGROUND_TILE_LSB_FETCH_BEGIN, PPU_BACKGROUND_TILE_LSB_FETCH_END, std::vector<unsigned int>(PPU_BACKGROUND_TILE_LSB_FETCH_EXTRA.begin(), PPU_BACKGROUND_TILE_LSB_FETCH_EXTRA.end()), &PPU2C02::FetchBackgroundTileLSB);
	this->setupFetchFunctionTiming(PPU_BACKGROUND_TILE_MSB_FETCH_BEGIN, PPU_BACKGROUND_TILE_MSB_FETCH_END, std::vector<unsigned int>(PPU_BACKGROUND_TILE_MSB_FETCH_EXTRA.begin(), PPU_BACKGROUND_TILE_MSB_FETCH_EXTRA.end()), &PPU2C02::FetchBackgroundTileMSB);
	

	// X/Y increment functions
	this->setupFetchFunctionTiming(PPU_INCREMENT_COARSE_X_BEGIN, PPU_INCREMENT_COARSE_X_END, std::vector<unsigned int>(PPU_INCREMENT_COARSE_X_EXTRA.begin(), PPU_INCREMENT_COARSE_X_EXTRA.end()), &PPU2C02::IncrementCoarseX);

	for (unsigned int scanline = PPU_VISIBLE_SCANLINES_BEGIN; scanline < PPU_VISIBLE_SCANLINES_END; scanline++) {
		m_renderFuncs[scanline][PPU_INCREMENT_Y_DOT].push_back(&PPU2C02::IncrementY);
		m_renderFuncs[scanline][PPU_INCREMENT_RESET_X_DOT].push_back(&PPU2C02::ResetX);
		m_renderFuncs[scanline][PPU_INCREMENT_RESET_X_DOT].push_back(&PPU2C02::ClearBackgroundShiftRegister);
	}
	m_renderFuncs[PPU_PRERENDER_LINE][PPU_INCREMENT_Y_DOT].push_back(&PPU2C02::IncrementY);
	m_renderFuncs[PPU_PRERENDER_LINE][PPU_INCREMENT_RESET_X_DOT].push_back(&PPU2C02::ResetX);
	m_renderFuncs[PPU_PRERENDER_LINE][PPU_INCREMENT_RESET_X_DOT].push_back(&PPU2C02::ClearBackgroundShiftRegister);

	for (unsigned int dot = PPU_SET_Y_BEGIN; dot < PPU_SET_Y_END; dot++) {
		m_renderFuncs[PPU_PRERENDER_LINE][dot].push_back(&PPU2C02::SetY);
	}

	// Updating background shifters
	for (unsigned int scanline = PPU_VISIBLE_SCANLINES_BEGIN; scanline < PPU_VISIBLE_SCANLINES_END; scanline++) {
		for (unsigned int dot = PPU_INCREMENT_COARSE_X_BEGIN; dot < PPU_INCREMENT_COARSE_X_END; dot += PPU_BACKGROUND_FETCH_OFFSET) {
			m_renderFuncs[scanline][dot].push_back(&PPU2C02::UpdateBackgroundShiftRegister);
		}
		for (unsigned int dot : PPU_INCREMENT_COARSE_X_EXTRA) {
			m_renderFuncs[scanline][dot].push_back(&PPU2C02::UpdateBackgroundShiftRegister);
		}
	}
	for (unsigned int dot : PPU_INCREMENT_COARSE_X_EXTRA) {
		m_renderFuncs[PPU_PRERENDER_LINE][dot].push_back(&PPU2C02::UpdateBackgroundShiftRegister);
	}

	// Secondary OAM
	for (unsigned int scanline = PPU_VISIBLE_SCANLINES_BEGIN; scanline < PPU_VISIBLE_SCANLINES_END; scanline++) {
		for (unsigned int dot = 0; dot < PPU_NUM_DOTS_PER_SCANLINE; dot++) {
			m_renderFuncs[scanline][dot].push_back(&PPU2C02::SecondaryOAMClock);
		}
	}
	for (unsigned int dot = PPU_VISIBLE_DOT_END; dot < PPU_NUM_DOTS_PER_SCANLINE; dot++) {
		m_renderFuncs[PPU_PRERENDER_LINE][dot].push_back(&PPU2C02::SecondaryOAMClock);
	}

	// VBland and flags funcs
	m_renderFuncs[PPU_NMI_SCANLINE][PPU_NMI_DOT].push_back(&PPU2C02::SetVBlank);
	m_renderFuncs[PPU_CLEAR_FLAGS_SCANLINE][PPU_CLEAR_FLAGS_DOT].push_back(&PPU2C02::ClearSTATUSFlags);

	// Push frame to display
	m_renderFuncs[PPU_DISPLAY_FRAME_SCANLINE][PPU_DISPLAY_FRAME_DOT].push_back(&PPU2C02::UpdateDisplay);
}

void PPU2C02::setupFetchFunctionTiming(unsigned int beginDot, unsigned int endDot, std::vector<unsigned int> extraDots, std::function<void(PPU2C02&)> renderFunc) {
	for (unsigned int scanline = PPU_VISIBLE_SCANLINES_BEGIN; scanline < PPU_VISIBLE_SCANLINES_END; scanline++) {
		for (unsigned int dot = beginDot; dot < endDot; dot += PPU_BACKGROUND_FETCH_OFFSET) {
			m_renderFuncs[scanline][dot].push_back(renderFunc);
		}
		for (unsigned int dot : extraDots) {
			m_renderFuncs[scanline][dot].push_back(renderFunc);
		}
	}
	for (unsigned int dot = beginDot; dot < endDot; dot += PPU_BACKGROUND_FETCH_OFFSET) {
		m_renderFuncs[PPU_PRERENDER_LINE][dot].push_back(renderFunc);
	}
	for (unsigned int dot : extraDots) {
		m_renderFuncs[PPU_PRERENDER_LINE][dot].push_back(renderFunc);
	}
}

bool PPU2C02::RenderingEnabled() const {
	return m_PPUMASK.flags.renderBackground || m_PPUMASK.flags.renderSprites;
}

bool PPU2C02::IsRendering() const {
	return this->RenderingEnabled() &&
		((m_scanline >= PPU_VISIBLE_SCANLINES_BEGIN && m_scanline < PPU_VISIBLE_SCANLINES_END)
			|| m_scanline == PPU_PRERENDER_LINE);
}

void PPU2C02::RenderPixel() {
	// At the moment, only render the background

	// Fetch background colour
	PixelColourInfo info = m_backgroundShiftRegister[m_fineX];
	uint16_t colourAddress = m_paletteRAM->GetBackgroundPaletteAddress(info.paletteIndex, info.colourIndex);
	uint8_t colourEntry = m_ppuBus->Read(colourAddress);

	// Choose if to render pixel or not
	if (this->IsRendering() && m_PPUMASK.flags.renderBackground && (m_dot >= PPU_LEFTSIDE_MASK_DOT || m_PPUMASK.flags.showLeftmostBackground)) {
		m_picture[m_scanline][m_dot - 1] = GetColourFromPalette(colourEntry, m_PPUMASK.flags.emphasizeRed, m_PPUMASK.flags.emphasizeGreen, m_PPUMASK.flags.emphasizeBlue);
		
	}
	else {
		m_picture[m_scanline][m_dot - 1] = GetColourFromPalette(0x00, m_PPUMASK.flags.emphasizeRed, m_PPUMASK.flags.emphasizeGreen, m_PPUMASK.flags.emphasizeBlue);
	}

	// Shift background register in preparation for the next pixel
	m_backgroundShiftRegister.Shift();
	
}

void PPU2C02::IncrementCoarseX() {
	if (!this->IsRendering()) {
		// X not incremented if not rendering
		return;
	}
	if (m_VRAMAddress.scrollFlags.coarseX == NAMETABLE_NUM_X_TILES - 1) {
		m_VRAMAddress.scrollFlags.coarseX = 0;
		// Flip horizontal nametable
		m_VRAMAddress.scrollFlags.nametableX = ~m_VRAMAddress.scrollFlags.nametableX;
	}
	else {
		m_VRAMAddress.scrollFlags.coarseX++;
	}
}

void PPU2C02::IncrementY() {
	if (!this->IsRendering()) {
		// Y not incremented if not rendering
		return;
	}
	if (m_VRAMAddress.scrollFlags.fineY == NAMETABLE_TILE_HEIGHT - 1) {
		m_VRAMAddress.scrollFlags.fineY = 0;
		if (m_VRAMAddress.scrollFlags.coarseY == NAMETABLE_NUM_Y_TILES - 1) {
			m_VRAMAddress.scrollFlags.coarseY = 0;
			// Flip vertical nametable
			m_VRAMAddress.scrollFlags.nametableY = ~m_VRAMAddress.scrollFlags.nametableY;
		}
		else if (m_VRAMAddress.scrollFlags.coarseY == NAMETABLE_COARSE_Y_MAX) {
			// Wraparound coarse y but don't switch nametables
			m_VRAMAddress.scrollFlags.coarseY = 0;
		}
		else {
			m_VRAMAddress.scrollFlags.coarseY++;
		}
	}
	else {
		m_VRAMAddress.scrollFlags.fineY++;
	}
}

void PPU2C02::ResetX() {
	if (!this->IsRendering()) {
		// X is reset only if rendering
		return;
	}
	m_VRAMAddress.scrollFlags.coarseX = m_TRAMAddress.scrollFlags.coarseX;
	m_VRAMAddress.scrollFlags.nametableX = m_TRAMAddress.scrollFlags.nametableX;
}

void PPU2C02::SetY() {
	if (!this->IsRendering()) {
		// Next frame's Y is set only if rendering
		return;
	}
	m_VRAMAddress.scrollFlags.coarseY = m_TRAMAddress.scrollFlags.coarseY;
	m_VRAMAddress.scrollFlags.nametableY = m_TRAMAddress.scrollFlags.nametableY;
	m_VRAMAddress.scrollFlags.fineY = m_TRAMAddress.scrollFlags.fineY;
}


void PPU2C02::FetchTileIDFromNametable() {
	m_nextBackgroundTileInfo.tileID = m_ppuBus->Read(NametableDevice::GetNametableByteAddress(m_VRAMAddress));
}

void PPU2C02::FetchPaletteIndexFromNametable() {
	uint8_t attributeByte = m_ppuBus->Read(NametableDevice::GetAttributeByteAddress(m_VRAMAddress));
	m_nextBackgroundTileInfo.paletteIndex = NametableDevice::GetPaletteFromAttributeByte(m_VRAMAddress, attributeByte);
}

void PPU2C02::FetchBackgroundTileLSB() {
	uint16_t address = PatternTableDevice::GetTileLowBitsAddress(m_PPUCTRL.flags.backgroundPatternTable, m_nextBackgroundTileInfo.tileID, m_VRAMAddress.scrollFlags.fineY);
	m_nextBackgroundTileInfo.coloursLSB = m_ppuBus->Read(address);
}

void PPU2C02::FetchBackgroundTileMSB() {
	uint16_t address = PatternTableDevice::GetTileHighBitsAddress(m_PPUCTRL.flags.backgroundPatternTable, m_nextBackgroundTileInfo.tileID, m_VRAMAddress.scrollFlags.fineY);
	m_nextBackgroundTileInfo.coloursMSB = m_ppuBus->Read(address);
}

void PPU2C02::UpdateBackgroundShiftRegister() {
	std::array<uint8_t, NAMETABLE_TILE_WIDTH> colours = PatternTableDevice::GetRowColourIndices(m_nextBackgroundTileInfo.coloursLSB, m_nextBackgroundTileInfo.coloursMSB);
	std::vector<PixelColourInfo> pixelColourInfo(8);
	for (unsigned int i = 0; i < NAMETABLE_TILE_WIDTH; i++) {
		pixelColourInfo[i].paletteIndex = m_nextBackgroundTileInfo.paletteIndex;
		pixelColourInfo[i].colourIndex = colours[i];
	}
	m_backgroundShiftRegister.InsertElements(pixelColourInfo);
}

void PPU2C02::ClearBackgroundShiftRegister() {
	m_backgroundShiftRegister.Clear();
}

void PPU2C02::SecondaryOAMClock() {
	if (m_scanline == 127) {
		uint8_t value = 0;
		value++;
	}
	m_secondaryOAM.Clock(m_scanline, m_dot, m_PPUCTRL.flags.spriteSize);
}

void PPU2C02::SetVBlank() {
	m_PPUSTATUS.flags.VBlank = 1;
	if (m_PPUCTRL.flags.NMIEnabled) {
		m_cpu->RaiseNMI();
	}
}

void  PPU2C02::ClearSTATUSFlags() {
	m_PPUSTATUS.flags.spriteOverflow = 0;
	m_PPUSTATUS.flags.sprite0Hit = 0;
	m_PPUSTATUS.flags.VBlank = 0;
}

void PPU2C02::UpdateDisplay() {
	if (m_environment != nullptr) {
		m_environment->UpdateDisplay(m_picture);
	}
}
