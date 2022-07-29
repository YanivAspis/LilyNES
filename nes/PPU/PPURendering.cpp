#include "PPU2C02.h"
#include "Nametabledevice.h"


bool PPU2C02::RenderingEnabled() const {
	return m_PPUMASK.flags.renderBackground || m_PPUMASK.flags.renderSprites;
}

bool PPU2C02::IsRendering() const {
	return this->RenderingEnabled() &&
		((m_scanline >= PPU_VISIBLE_SCANLINES_BEGIN && m_scanline < PPU_VISIBLE_SCANLINES_END)
			|| m_scanline == PPU_PRERENDER_LINE);
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

void PPU2C02::IncrementCoarseX() {
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