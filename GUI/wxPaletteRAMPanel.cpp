//#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include "wxPaletteRAMPanel.h"


wxDEFINE_EVENT(EVT_PALETTE_GUI_UPDATE, wxNESStateEvent<PaletteRAMState>);
wxBEGIN_EVENT_TABLE(wxPaletteRAMPanel, wxPanel)
	EVT_PAINT(wxPaletteRAMPanel::OnPaint)
	EVT_SIZE(wxPaletteRAMPanel::OnSize)
	EVT_ERASE_BACKGROUND(wxPaletteRAMPanel::OnBackgroundErase)
wxEND_EVENT_TABLE()

wxPaletteRAMPanel::wxPaletteRAMPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	this->Bind(EVT_PALETTE_GUI_UPDATE, &wxPaletteRAMPanel::OnPaletteUpdate, this);
	m_selectedPalette = 0;
	m_topMargin = 0;
	m_leftMargin = 0;
	m_colourHeight = 0;
	m_colourWidth = 0;
	m_betweenColoursMargin = 0;
	this->SetBackgroundColour(wxColour(*wxWHITE));
}

void wxPaletteRAMPanel::OnPaint(wxPaintEvent& evt) {
	wxBufferedPaintDC dc(this);
	wxSize windowSize = this->GetClientSize();
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	dc.DrawRectangle(0, 0, windowSize.GetWidth(), windowSize.GetHeight());

	wxPen selectedPen = *wxYELLOW;
	wxPen unselectedPen = *wxGREY_PEN;
	for (unsigned int palette = 0; palette < PALETTE_NUM_BACKGROUND_PALETTES; palette++) {
		if (m_selectedPalette == palette) {
			dc.SetPen(selectedPen);
		}
		else {
			dc.SetPen(unselectedPen);
		}
		NESPixel colour;
		if (palette == 0) {
			colour = GetColourFromPalette(m_currState.universalBackgroundColour, 0, 0, 0);
		}
		else {
			colour = GetColourFromPalette(m_currState.otherBackgroundColours[palette - 1], 0, 0, 0);
		}
		dc.SetBrush(wxBrush(wxColour(colour.red, colour.green, colour.blue)));
		unsigned int topLeftX = m_leftMargin;
		unsigned int topLeftY = m_topMargin + (m_colourHeight + m_betweenColoursMargin) * palette;
		dc.DrawRectangle(topLeftX, topLeftY, m_colourWidth, m_colourHeight);

		for (unsigned int index = 0; index < PALETTE_NUM_PALETTE_COLOURS; index++) {
			NESPixel colour = GetColourFromPalette(m_currState.backgroundPalettes[palette][index], 0, 0, 0);
			dc.SetBrush(wxBrush(wxColour(colour.red, colour.green, colour.blue)));
			unsigned int topLeftX = m_leftMargin + m_colourWidth * (index+1);
			unsigned int topLeftY = m_topMargin + (m_colourHeight + m_betweenColoursMargin) * palette;
			dc.DrawRectangle(topLeftX, topLeftY, m_colourWidth, m_colourHeight);
		}
	}
	for (unsigned int palette = 0; palette < PALETTE_NUM_SPRITE_PALETTES; palette++) {
		if (m_selectedPalette == palette + PALETTE_NUM_BACKGROUND_PALETTES) {
			dc.SetPen(selectedPen);
		}
		else {
			dc.SetPen(unselectedPen);
		}
		NESPixel colour;
		if (palette == 0) {
			colour = GetColourFromPalette(m_currState.universalBackgroundColour, 0, 0, 0);
		}
		else {
			colour = GetColourFromPalette(m_currState.otherBackgroundColours[palette - 1], 0, 0, 0);
		}
		dc.SetBrush(wxBrush(wxColour(colour.red, colour.green, colour.blue)));
		unsigned int topLeftX = m_leftMargin;
		unsigned int topLeftY = m_topMargin + (m_colourHeight + m_betweenColoursMargin) * PALETTE_NUM_BACKGROUND_PALETTES + (m_colourHeight + m_betweenColoursMargin) * palette;
		dc.DrawRectangle(topLeftX, topLeftY, m_colourWidth, m_colourHeight);

		for (unsigned int index = 0; index < PALETTE_NUM_PALETTE_COLOURS; index++) {
			NESPixel colour = GetColourFromPalette(m_currState.spritePalettes[palette][index], 0, 0, 0);
			dc.SetBrush(wxBrush(wxColour(colour.red, colour.green, colour.blue)));
			unsigned int topLeftX = m_leftMargin + m_colourWidth * (index + 1);
			unsigned int topLeftY = m_topMargin + (m_colourHeight + m_betweenColoursMargin) * PALETTE_NUM_BACKGROUND_PALETTES + (m_colourHeight + m_betweenColoursMargin) * palette;
			dc.DrawRectangle(topLeftX, topLeftY, m_colourWidth, m_colourHeight);
		}
	}
}

void wxPaletteRAMPanel::OnSize(wxSizeEvent& evt) {
	wxSize size = evt.GetSize();
	unsigned int windowHeight = size.GetHeight() * 0.75;
	unsigned int windowWidth = size.GetWidth() * 0.75;
	m_topMargin = std::max((windowHeight * 3) / 100, (unsigned int)3);
	m_colourHeight = std::max(windowHeight / 10, (unsigned int)10);
	m_betweenColoursMargin = std::max((windowHeight * 7) / 100, (unsigned int)7);
	m_colourWidth = m_colourHeight;
	m_leftMargin = std::max((unsigned int)((windowWidth - m_colourWidth) / 2), (unsigned int)0);
	this->Refresh();
	evt.Skip();
}

void wxPaletteRAMPanel::OnPaletteUpdate(wxNESStateEvent<PaletteRAMState>& evt) {
	m_currState = evt.GetState();
	this->Refresh();
}

void wxPaletteRAMPanel::OnBackgroundErase(wxEraseEvent& evt) {}

void wxPaletteRAMPanel::SelectNextPalette() {
	m_selectedPalette++;
	m_selectedPalette %= (PALETTE_NUM_BACKGROUND_PALETTES + PALETTE_NUM_SPRITE_PALETTES);
	this->Refresh();
}