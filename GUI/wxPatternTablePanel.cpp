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
	wxSize windowSize = this->GetClientSize();
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	dc.DrawRectangle(0, 0, windowSize.GetWidth(), windowSize.GetHeight());

	this->DrawPatternTable(dc, m_currState.patternTableState.patternTable0, m_currState.paletteRAMState, 0, 0);
	this->DrawPatternTable(dc, m_currState.patternTableState.patternTable1, m_currState.paletteRAMState, m_widthScaleFactor * windowSize.GetWidth(), 0);
}

void wxPatternTablePanel::OnSize(wxSizeEvent& evt) {
	wxSize currSize = evt.GetSize();
	m_widthScaleFactor = (double)currSize.GetWidth() / (double)PATTERN_TABLE_IMAGE_WIDTH / 2;
	m_heightScaleFactor = (double)currSize.GetHeight() / (double)PATTERN_TABLE_IMAGE_HEIGHT;
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

void wxPatternTablePanel::DrawPatternTable(wxBufferedPaintDC& dc, std::array<uint8_t, PATTERN_TABLE_SIZE> patternTable, PaletteRAMState paletteRAM, unsigned int x, unsigned int y) {
	wxImage* image = new wxImage(PATTERN_TABLE_IMAGE_WIDTH, PATTERN_TABLE_IMAGE_HEIGHT);
	for (unsigned int y = 0; y < PATTERN_TABLE_IMAGE_HEIGHT; y++) {
		for (unsigned int x = 0; x < PATTERN_TABLE_IMAGE_WIDTH; x++) {
			uint8_t tileIndex = (y / PATTERN_TABLE_NUM_TILES_HEIGHT) * PATTERN_TABLE_NUM_TILES_WIDTH + x / PATTERN_TABLE_NUM_TILES_WIDTH;
			unsigned int rowIndex = y % PATTERN_TABLE_TILE_HEIGHT;
			unsigned int colIndex = 7 - (x % PATTERN_TABLE_TILE_WIDTH);
			uint8_t lsb = TestBit8(patternTable[tileIndex * 2 * PATTERN_TABLE_TILE_HEIGHT + rowIndex], colIndex);
			uint8_t msb = TestBit8(patternTable[tileIndex * 2 * PATTERN_TABLE_TILE_HEIGHT + rowIndex + PATTERN_TABLE_MSB_OFFSET], colIndex);
			uint8_t colourIndex = ShiftLeft8(msb, 1) | lsb;
			NESPixel colour = GetColour(paletteRAM, colourIndex);
			image->SetRGB(x, y, colour.red, colour.green, colour.blue);
		}
	}
	dc.SetUserScale(m_widthScaleFactor, m_heightScaleFactor);
	if ((*image).IsOk()) {
		dc.DrawBitmap(*image, 0, 0, false);
	}
	delete image;
	image = nullptr;
}

NESPixel wxPatternTablePanel::GetColour(PaletteRAMState paletteRAM, uint8_t colourIndex) {
	uint8_t colourEntry;
	if (colourIndex % (PALETTE_NUM_PALETTE_COLOURS + 1) == 0) {
		colourEntry = paletteRAM.universalBackgroundColour;
	}
	else if (m_selectedPalette < PALETTE_NUM_BACKGROUND_PALETTES) {
		colourEntry = paletteRAM.backgroundPalettes[m_selectedPalette][colourIndex - 1];
	}
	else {
		colourEntry = paletteRAM.spritePalettes[m_selectedPalette - PALETTE_NUM_BACKGROUND_PALETTES][colourIndex - 1];
	}
	return GetColourFromPalette(colourEntry, 0, 0, 0);
}
