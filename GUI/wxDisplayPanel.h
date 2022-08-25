#pragma once

#include <chrono>
#include <SDL.h>
#include <SDL_ttf.h>

#include "wx/wx.h"
#include "../nes/PPU/NESPicture.h"

class wxDisplayPanel : public wxPanel {
public:
	wxDisplayPanel(wxWindow* parent, int id = wxID_ANY);
	~wxDisplayPanel();
	void OnPaint(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void OnImageUpdate(wxThreadEvent& evt);
	void ClearDisplay();
	
	void SetImage(const NESPicture& picture);
	NESPicture GetImage();

	void ToggleRefreshRate();

private:
	NESPicture m_picture;
	wxCriticalSection m_pictureCritSection;

	SDL_Window* m_sdlWindow;
	SDL_Renderer* m_renderer;
	SDL_Surface* m_surface;
	SDL_Texture* m_texture;

	TTF_Font* m_refreshRateFont;
	SDL_Colour m_refreshRateColour;
	SDL_Rect m_refreshRateRect;

	bool m_displayCleared;
	bool m_displayRefreshRate;
	unsigned int m_frameCount;
	std::chrono::steady_clock m_clock;
	std::chrono::steady_clock::time_point m_startTime;
};