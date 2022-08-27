#pragma once

#include <chrono>
#include <SDL.h>
#include <SDL_ttf.h>

#include "wx/wx.h"
#include "../nes/PPU/NESPicture.h"

constexpr double DISPLAY_TARGET_REFRESH_RATE = 60.0;

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
	void SetUserMessage(std::string message, double duration);
	void ClearUserMessage();

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

	TTF_Font* m_userMessageFont;
	SDL_Colour m_userMessageColour;
	SDL_Rect m_userMessageRect;
	std::string m_userMessage;
	int m_userMessageFramesLeft;

	bool m_displayCleared;
	bool m_displayRefreshRate;
	unsigned int m_frameCount;
	std::chrono::steady_clock m_clock;
	std::chrono::steady_clock::time_point m_startTime;
};