#pragma once

#include "wx/wx.h"
#include "wxNESStateEvent.h"
#include "../nes/PPU/Palette.h"
#include "../nes/PPU/PaletteRAMDevice.h"

wxDECLARE_EVENT(EVT_PALETTE_GUI_UPDATE, wxNESStateEvent<PaletteRAMState>);

class wxPaletteRAMPanel : public wxPanel {
public:
	wxPaletteRAMPanel(wxWindow* parent);
	void OnPaint(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void OnPaletteUpdate(wxNESStateEvent<PaletteRAMState>& evt);
	void OnBackgroundErase(wxEraseEvent& evt);
	void SelectNextPalette();

private:
	PaletteRAMState m_currState;
	unsigned int m_selectedPalette;

	unsigned int m_topMargin;
	unsigned int m_leftMargin;
	unsigned int m_colourHeight;
	unsigned int m_colourWidth;
	unsigned int m_betweenColoursMargin;


	wxDECLARE_EVENT_TABLE();
};