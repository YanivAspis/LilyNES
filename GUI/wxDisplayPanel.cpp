#include "wxDisplayPanel.h"
#include "../utils/NESUtils.h"

using namespace NESUtils;


wxDisplayPanel::wxDisplayPanel(wxWindow* parent, int id) : wxPanel(parent, id) { //, m_image(NES_PICTURE_WIDTH, NES_PICTURE_HEIGHT) {
	Bind(wxEVT_PAINT, &wxDisplayPanel::OnPaint, this);
	Bind(wxEVT_SIZE, &wxDisplayPanel::OnSize, this);
	Bind(wxEVT_THREAD, &wxDisplayPanel::OnImageUpdate, this);

	m_displayCleared = true;
	m_frameCount = 0;
	m_displayRefreshRate = false;
	ResetNESPicture(m_picture);

	m_sdlWindow = SDL_CreateWindowFrom(this->GetHandle());
	if (m_sdlWindow == nullptr) {
		throw SDLException("Display window could not be created.");
	}
	m_renderer = SDL_CreateRenderer(m_sdlWindow, -1, SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr) {
		throw SDLException("Renderer could not be created.");
	}
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	m_surface = SDL_CreateRGBSurface(0, NES_PICTURE_WIDTH, NES_PICTURE_HEIGHT, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	m_texture = nullptr;

	if (TTF_Init() < 0) {
		throw SDLException("Error initializing TTF.");
	}

	m_refreshRateFont = TTF_OpenFont("fonts/FreeSansBold.ttf", 36);
	m_refreshRateColour = { 255, 255, 0, 255 };
	m_refreshRateRect.x = 1;
	m_refreshRateRect.y = 0;
	m_refreshRateRect.w = 0;
	m_refreshRateRect.h = 0;
}

wxDisplayPanel::~wxDisplayPanel() {
	SDL_FreeSurface(m_surface);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_sdlWindow);
	TTF_Quit();
}


void wxDisplayPanel::OnPaint(wxPaintEvent& evt) {
	// This defaults the display to all black if the emulator is not drawing anything
	wxPaintDC dc(this);
	if (m_displayCleared) {
		this->ClearDisplay();
	}
}

void wxDisplayPanel::OnSize(wxSizeEvent& evt) {
	wxSize size = evt.GetSize();
	m_refreshRateRect.w = size.GetWidth() / 6;
	m_refreshRateRect.h = size.GetHeight() / 8;
	evt.Skip();
}

void wxDisplayPanel::OnImageUpdate(wxThreadEvent& evt) {
	SDL_LockSurface(m_surface);
	{
		wxCriticalSectionLocker enter(m_pictureCritSection);
		memcpy(m_surface->pixels, m_picture.data(), NES_PICTURE_WIDTH * NES_PICTURE_HEIGHT * 3);
	}
	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	SDL_UnlockSurface(m_surface);
	SDL_RenderClear(m_renderer);
	SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
	SDL_DestroyTexture(m_texture);

	if (m_displayRefreshRate) {
		m_frameCount++;
		std::chrono::duration<long long, std::nano> runtime = m_clock.now() - m_startTime;
		double seconds = runtime.count() / 1000000000.0;
		double refreshRate = m_frameCount / seconds;

		SDL_Surface* refreshRateSurface = TTF_RenderText_Solid(m_refreshRateFont, wxString::Format("%.2f", refreshRate), m_refreshRateColour);
		SDL_Texture* refreshRateTexture = SDL_CreateTextureFromSurface(m_renderer, refreshRateSurface);

		SDL_RenderCopy(m_renderer, refreshRateTexture, nullptr, &m_refreshRateRect);
		SDL_DestroyTexture(refreshRateTexture);
		SDL_FreeSurface(refreshRateSurface);
	}

	SDL_RenderPresent(m_renderer);
	m_displayCleared = false;
}

void wxDisplayPanel::ClearDisplay() {
	SDL_RenderClear(m_renderer);
	SDL_RenderPresent(m_renderer);
	m_displayCleared = true;
}

void wxDisplayPanel::SetImage(const NESPicture& picture) {
	{
		wxCriticalSectionLocker enter(m_pictureCritSection);
		m_picture = picture;
	}
	// Signal to the main thread that the picture has been updated
	wxQueueEvent(this, new wxThreadEvent());
}

NESPicture wxDisplayPanel::GetImage() {
	wxCriticalSectionLocker enter(m_pictureCritSection);
	return m_picture;
}

void wxDisplayPanel::ToggleRefreshRate() {
	m_displayRefreshRate = !m_displayRefreshRate;
	m_frameCount = 0;
	m_startTime = m_clock.now();
}