#pragma once

#include <chrono>

#include "wx/wx.h"
#include "../nes/PPU/NESPicture.h"

class wxDisplayPanel : public wxPanel {
public:
	wxDisplayPanel(wxWindow* parent, int id = wxID_ANY);
	void Render(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void OnImageUpdate(wxThreadEvent& evt);
	void OnBackgroundErase(wxEraseEvent& evt);
	
	void SetImage(const NESPicture& picture);
	NESPicture GetImage();

	void ToggleRefreshRate();

private:
	NESPicture m_picture;
	wxCriticalSection m_pictureCritSection;

	double m_widthScaleFactor;
	double m_heightScaleFactor;

	bool m_displayRefreshRate;
	unsigned int m_frameCount;
	std::chrono::steady_clock m_clock;
	std::chrono::steady_clock::time_point m_startTime;
};