#include <wx/dcbuffer.h>
#include "wxPatternTablePanel.h"
#include "../utils/BitwiseUtils.h"
#include "../nes/PPU/Palette.h"

using namespace BitwiseUtils;

wxDEFINE_EVENT(EVT_PATTERN_TABLE_UPDATE, wxNESStateEvent<PatternPaletteState>);
wxBEGIN_EVENT_TABLE(wxPatternTablePanel, wxPanel)
	EVT_PAINT(wxPatternTablePanel::OnPaint)
	EVT_SIZE(wxPatternTablePanel::OnSize)
	EVT_ERASE_BACKGROUND(wxPatternTablePanel::OnBackgroundErase)
wxEND_EVENT_TABLE()


wxPatternTablePanel::wxPatternTablePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	this->Bind(EVT_PATTERN_TABLE_UPDATE, &wxPatternTablePanel::OnPatternTableUpdate, this);
	m_selectedPalette = 0;
	m_widthScaleFactor = 0;
	m_heightScaleFactor = 0;
}

void wxPatternTablePanel::OnPaint(wxPaintEvent& evt) {
	wxBufferedPaintDC dc(this);

	// Draw background
	wxSize windowSize = this->GetSize();
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	dc.DrawRectangle(0, 0, windowSize.GetWidth(), windowSize.GetHeight());

	this->DrawPatternTable(dc, 0, 0, 0);
	this->DrawPatternTable(dc, 1, windowSize.GetWidth() / 3, 0);
}

void wxPatternTablePanel::OnSize(wxSizeEvent& evt) {
	wxSize currSize = evt.GetSize();
	m_widthScaleFactor = (double)currSize.GetWidth() / (double)PATTERN_TABLE_IMAGE_WIDTH / 2 / 1.2;
	m_heightScaleFactor = (double)currSize.GetHeight() / (double)PATTERN_TABLE_IMAGE_HEIGHT / 1.2;
	this->Refresh();
	evt.Skip();
}

void wxPatternTablePanel::OnBackgroundErase(wxEraseEvent& evt) {}

void wxPatternTablePanel::OnPatternTableUpdate(wxNESStateEvent<PatternPaletteState>& evt) {
	m_currState = evt.GetState();
	this->Refresh();
}

void wxPatternTablePanel::SelectNextPalette() {
	m_selectedPalette++;
	m_selectedPalette %= (PALETTE_NUM_BACKGROUND_PALETTES + PALETTE_NUM_SPRITE_PALETTES);
	this->Refresh();
}

void wxPatternTablePanel::DrawPatternTable(wxBufferedPaintDC& dc, unsigned int patternTableID, unsigned int x, unsigned int y) {
	std::array<uint8_t, PATTERN_TABLE_SIZE> patternTable = patternTableID == 0 ? m_currState.patternTableState.patternTable0 : m_currState.patternTableState.patternTable1;
	
	wxImage* image = new wxImage(PATTERN_TABLE_IMAGE_WIDTH, PATTERN_TABLE_IMAGE_HEIGHT);
	for (unsigned int tileY = 0; tileY < PATTERN_TABLE_NUM_TILES_HEIGHT; tileY++) {
		for (unsigned int tileX = 0; tileX < PATTERN_TABLE_NUM_TILES_WIDTH; tileX++) {

			uint8_t tileID = tileY * PATTERN_TABLE_NUM_TILES_WIDTH + tileX;
			for (unsigned int row = 0; row < PATTERN_TABLE_TILE_HEIGHT; row++) {

				unsigned int drawY = tileY * PATTERN_TABLE_TILE_HEIGHT + row;
				uint16_t LSBIndex = ClearUpperBits16(PatternTableDevice::GetTileLowBitsAddress(patternTableID, tileID, row), 12);
				uint16_t MSBIndex = ClearUpperBits16(PatternTableDevice::GetTileHighBitsAddress(patternTableID, tileID, row), 12);
				uint8_t LSB = patternTable[LSBIndex];
				uint8_t MSB = patternTable[MSBIndex];
				std::array<uint8_t, PATTERN_TABLE_TILE_WIDTH> colourIndices = PatternTableDevice::GetRowColourIndices(LSB, MSB);
				for (unsigned int col = 0; col < PATTERN_TABLE_TILE_WIDTH; col++) {
					unsigned int drawX = tileX * PATTERN_TABLE_TILE_WIDTH + col;
					NESPixel colour = this->GetColour(colourIndices[col]);
					image->SetRGB(drawX, drawY, colour.red, colour.green, colour.blue);
				}
			}
		}
	}
	dc.SetUserScale(m_widthScaleFactor, m_heightScaleFactor);
	if ((*image).IsOk()) {
		dc.DrawBitmap(*image, x, y, false);
	}
	delete image;
	image = nullptr;
}

NESPixel wxPatternTablePanel::GetColour(uint8_t colourIndex) {
	uint8_t colourEntry;
	if (colourIndex % (PALETTE_NUM_PALETTE_COLOURS + 1) == 0) {
		colourEntry = m_currState.paletteRAMState.universalBackgroundColour;
	}
	else if (m_selectedPalette < PALETTE_NUM_BACKGROUND_PALETTES) {
		colourEntry = m_currState.paletteRAMState.backgroundPalettes[m_selectedPalette][colourIndex - 1];
	}
	else {
		colourEntry = m_currState.paletteRAMState.spritePalettes[m_selectedPalette - PALETTE_NUM_BACKGROUND_PALETTES][colourIndex - 1];
	}
	return GetColourFromPalette(colourEntry, 0, 0, 0);
}
