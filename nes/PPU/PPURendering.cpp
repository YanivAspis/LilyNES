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

SpritePixelInfo::SpritePixelInfo() {
	paletteIndex = 0;
	colourIndex = 0;
	spriteID = OAM_NO_SPRITE_ID;
	backgroundPriority = true;
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

		// Skip (0,0) cycle on odd frames if rendering is enabled
		if (m_frameCount % 2 == 1 && this->RenderingEnabled()) {
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
		m_renderFuncs[scanline][SECONDARY_OAM_INTERNAL_INITIALIZATION_DOT].push_back(&PPU2C02::SecondaryOAMInitialization);
		m_renderFuncs[scanline][SECONDARY_OAM_EVALUATION_END_DOT].push_back(&PPU2C02::SecondaryOAMSpriteEvaluation);
	}

	// Fetching sprite data
	this->setupFetchFunctionTiming(PPU_GARBAGE_TILE_ID_FETCH_BEGIN, PPU_GARBAGE_TILE_ID_FETCH_END, std::vector<unsigned int>(), &PPU2C02::GarbageFetchTileIDFromNametable);
	this->setupFetchFunctionTiming(PPU_GARBAGE_ATTRIBUTE_FETCH_BEGIN, PPU_GARBAGE_ATTRIBUTE_FETCH_END, std::vector<unsigned int>(), &PPU2C02::GarbageFetchPaletteIndexFromNametable);
	this->setupFetchFunctionTiming(PPU_SPRITE_TILE_LSB_FETCH_BEGIN, PPU_SPRITE_TILE_LSB_FETCH_END, std::vector<unsigned int>(), &PPU2C02::FetchSpriteLSB);
	this->setupFetchFunctionTiming(PPU_SPRITE_TILE_MSB_FETCH_BEGIN, PPU_SPRITE_TILE_MSB_FETCH_END, std::vector<unsigned int>(), &PPU2C02::FetchSpriteMSB);
	for (unsigned int scanline = PPU_VISIBLE_SCANLINES_BEGIN; scanline < PPU_VISIBLE_SCANLINES_END; scanline++) {
		m_renderFuncs[scanline][PPU_UPDATE_SPRITE_LINE_DOT].push_back(&PPU2C02::ClearSpritesLine);
		m_renderFuncs[scanline][PPU_UPDATE_SPRITE_LINE_DOT].push_back(&PPU2C02::RenderToSpriteLine);
	}
	m_renderFuncs[PPU_PRERENDER_LINE][PPU_UPDATE_SPRITE_LINE_DOT].push_back(&PPU2C02::ClearSpritesStack); // Remove garbage data from sprites stack
	m_renderFuncs[PPU_PRERENDER_LINE][PPU_UPDATE_SPRITE_LINE_DOT].push_back(&PPU2C02::ClearSpritesLine); //  This makes sure there is nothing to render for the first scanline

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

bool PPU2C02::IsRenderingBackground() const {
	return (this->IsRendering()) && m_PPUMASK.flags.renderBackground && (m_dot >= PPU_LEFTSIDE_MASK_DOT || m_PPUMASK.flags.showLeftmostBackground);
}

bool PPU2C02::IsRenderingSprites() const {
	return (this->IsRendering()) && m_PPUMASK.flags.renderSprites && (m_dot >= PPU_LEFTSIDE_MASK_DOT || m_PPUMASK.flags.showLeftmostSprites);
}

void PPU2C02::RenderPixel() {
	// Fetch background and foreground colours
	PixelColourInfo backgroundInfo = m_backgroundShiftRegister[m_fineX];
	SpritePixelInfo spriteInfo = m_spriteLine[m_dot - 1];

	// Choose if rendering background or foreground
	uint8_t selectedPalette;
	uint8_t selectedColourIndex;
	this->GetPixelColour(backgroundInfo.paletteIndex, backgroundInfo.colourIndex, spriteInfo.paletteIndex, spriteInfo.colourIndex, spriteInfo.spriteID, spriteInfo.backgroundPriority, selectedPalette, selectedColourIndex);

	// Fetch colour and render
	uint16_t colourAddress = m_paletteRAM->GetBackgroundPaletteAddress(selectedPalette, selectedColourIndex);
	uint8_t colourEntry = m_ppuBus->Read(colourAddress);
	m_picture[m_scanline][m_dot - 1] = GetColourFromPalette(colourEntry, m_PPUMASK.flags.emphasizeRed, m_PPUMASK.flags.emphasizeGreen, m_PPUMASK.flags.emphasizeBlue);

	// Shift background register in preparation for the next pixel
	m_backgroundShiftRegister.Shift();	
}

void PPU2C02::GetPixelColour(uint8_t backgroundPalette, uint8_t backgroundColourIndex, uint8_t spritePalette, uint8_t spriteColourIndex, unsigned int spriteID, bool backgroundPriority, uint8_t& selectedPalette, uint8_t& selectedColourIndex) {
	if (!this->IsRenderingBackground()) {
		if (!this->IsRenderingSprites() || spriteID == OAM_NO_SPRITE_ID) {
			// Case 1 - nothing to render - either rendering disabled or background disabled and no sprite
			selectedPalette = 0;
			selectedColourIndex = 0;
			return;
		}
		// Case 2 - only sprites are rendered
		selectedPalette = spritePalette + PALETTE_NUM_BACKGROUND_PALETTES;
		selectedColourIndex = spriteColourIndex;
		return;
	}
	else if (!this->IsRenderingSprites()) {
		// Case 3 - only background is rendered
		selectedPalette = backgroundPalette;
		selectedColourIndex = backgroundColourIndex;
		return;
	}

	// Both background and sprites are rendering - check if they're transparent

	if (spriteColourIndex == 0) {
		// Case 4 - sprite is transparent, or both, so render background
		selectedPalette = backgroundPalette;
		selectedColourIndex = backgroundColourIndex;
		return;
	}
	else if (backgroundColourIndex == 0) {
		// Case 5 - background is transparent, so render sprite
		selectedPalette = spritePalette + PALETTE_NUM_BACKGROUND_PALETTES;
		selectedColourIndex = spriteColourIndex;
		return;
	}

	// Case 6 - both are opaque, so use background priority to resolve
	if (backgroundPriority) {
		selectedPalette = backgroundPalette;
		selectedColourIndex = backgroundColourIndex;
	}
	else {
		selectedPalette = spritePalette + PALETTE_NUM_BACKGROUND_PALETTES;
		selectedColourIndex = spriteColourIndex;
	}

	// Since both are opaque, check conditions for sprite 0 hit
	// TODO: Should this only be checked from dot 2?
	if (!m_PPUSTATUS.flags.sprite0Hit && spriteID == 0) {
		m_PPUSTATUS.flags.sprite0Hit = 1;
	}
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

void PPU2C02::GarbageFetchTileIDFromNametable() {
	m_ppuBus->Read(NametableDevice::GetNametableByteAddress(m_VRAMAddress));
}

void PPU2C02::GarbageFetchPaletteIndexFromNametable() {
	m_ppuBus->Read(NametableDevice::GetAttributeByteAddress(m_VRAMAddress));
}

void PPU2C02::FetchSpriteLSB() {
	unsigned int entryIndex = (m_dot - PPU_GARBAGE_TILE_ID_FETCH_BEGIN) / SECONDARY_OAM_SIZE;
	SecondaryOAMEntry entry = m_secondaryOAM.GetEntry(entryIndex);
	m_fetchedSpriteLSB = m_ppuBus->Read(PatternTableDevice::GetSpriteLowBitsAddress(m_PPUCTRL.flags.spritePatternTable, m_PPUCTRL.flags.spriteSize,
		entry.entry.tileID, entry.entry.y, m_scanline, entry.entry.attribute.flags.flipVertically));
}

void PPU2C02::FetchSpriteMSB() {
	unsigned int entryIndex = (m_dot - PPU_GARBAGE_TILE_ID_FETCH_BEGIN) / SECONDARY_OAM_SIZE;
	SecondaryOAMEntry entry = m_secondaryOAM.GetEntry(entryIndex);
	uint8_t fetchedSpriteMSB = m_ppuBus->Read(PatternTableDevice::GetSpriteHighBitsAddress(m_PPUCTRL.flags.spritePatternTable, m_PPUCTRL.flags.spriteSize,
		entry.entry.tileID, entry.entry.y, m_scanline, entry.entry.attribute.flags.flipVertically));

	std::array<uint8_t, PATTERN_TABLE_TILE_WIDTH> rowColours = PatternTableDevice::GetRowColourIndices(m_fetchedSpriteLSB, fetchedSpriteMSB);
	if (entry.entry.attribute.flags.flipHorizontally) {
		std::reverse(rowColours.begin(), rowColours.end());
	}

	SpriteRowInfo rowInfo;
	rowInfo.x = entry.entry.x;
	rowInfo.paletteIndex = entry.entry.attribute.flags.paletteIndex;
	rowInfo.rowColours = rowColours;
	rowInfo.spriteID = entry.spriteID;
	rowInfo.backgroundPriority = entry.entry.attribute.flags.backgroundPriority;
	m_spritesToRender.push_back(rowInfo);
}

void PPU2C02::ClearSpritesStack() {
	while (!m_spritesToRender.empty()) {
		m_spritesToRender.pop_back();
	}
}

void PPU2C02::ClearSpritesLine() {
	m_spriteLine.fill(SpritePixelInfo());
}

void PPU2C02::RenderToSpriteLine() {
	while (!m_spritesToRender.empty()) {
		SpriteRowInfo rowInfo = m_spritesToRender.back();
		for (unsigned int i = rowInfo.x; (i < rowInfo.x + PATTERN_TABLE_TILE_WIDTH) && i < NES_PICTURE_WIDTH; i++) {
			if (rowInfo.rowColours[i - rowInfo.x] != 0) {
				m_spriteLine[i].paletteIndex = rowInfo.paletteIndex;
				m_spriteLine[i].colourIndex = rowInfo.rowColours[i - rowInfo.x];
				m_spriteLine[i].spriteID = rowInfo.spriteID;
				m_spriteLine[i].backgroundPriority = rowInfo.backgroundPriority;
			}
		}
		m_spritesToRender.pop_back();
	}
}

void PPU2C02::SecondaryOAMInitialization() {
	m_secondaryOAM.SecondaryOAMInitialization();
}

void PPU2C02::SecondaryOAMSpriteEvaluation() {
	m_secondaryOAM.SpriteEvaluation(m_scanline, m_PPUCTRL.flags.spriteSize);
	if (m_secondaryOAM.SpriteOverflowDetected()) {
		m_PPUSTATUS.flags.spriteOverflow = 1;
	}
}

/*
void PPU2C02::SecondaryOAMClock() {
	m_secondaryOAM.Clock(m_scanline, m_dot, m_PPUCTRL.flags.spriteSize);
	if (m_secondaryOAM.SpriteOverflowDetected()) {
		m_PPUSTATUS.flags.spriteOverflow = 1;
	}
}*/

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