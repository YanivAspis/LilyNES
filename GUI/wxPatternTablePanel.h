#pragma once

#include <wx/wx.h>
#include "../nes/PPU/PatternTableDevice.h"
#include "../nes/PPU/PaletteRAMDevice.h"
#include "wxNESStateEvent.h"
#include "../nes/PPU/NESPicture.h"

constexpr unsigned int PATTERN_TABLE_IMAGE_WIDTH = PATTERN_TABLE_NUM_TILES_WIDTH * PATTERN_TABLE_TILE_WIDTH;
constexpr unsigned int PATTERN_TABLE_IMAGE_HEIGHT = PATTERN_TABLE_NUM_TILES_WIDTH * PATTERN_TABLE_TILE_HEIGHT;

struct PatternPaletteState {
	PatternTableState patternTableState;
	PaletteRAMState paletteRAMState;
};

wxDECLARE_EVENT(EVT_PATTERN_TABLE_UPDATE, wxNESStateEvent<PatternPaletteState>);

class wxPatternTablePanel : public wxPanel {
public:
	wxPatternTablePanel(wxWindow* parent);
	void OnPaint(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void OnBackgroundErase(wxEraseEvent& evt);
	void OnPatternTableUpdate(wxNESStateEvent<PatternPaletteState>& evt);
	void SelectNextPalette();

private:
	void DrawPatternTable(wxBufferedPaintDC& dc, std::array<uint8_t, PATTERN_TABLE_SIZE> patternTable, PaletteRAMState paletteRAM, unsigned int x, unsigned int y);
	NESPixel GetColour(PaletteRAMState paletteRAM, uint8_t colourIndex);

	PatternPaletteState m_currState;
	unsigned int m_selectedPalette;

	double m_widthScaleFactor;
	double m_heightScaleFactor;

	wxDECLARE_EVENT_TABLE();
};