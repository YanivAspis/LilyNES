#include "wxDisplayPanel.h"
#include "wx/dcbuffer.h"

wxDisplayPanel::wxDisplayPanel(wxWindow* parent, int id) : wxPanel(parent, id) {
	Bind(wxEVT_PAINT, &wxDisplayPanel::Render, this);
	Bind(wxEVT_SIZE, &wxDisplayPanel::OnSize, this);
	Bind(wxEVT_ERASE_BACKGROUND, &wxDisplayPanel::OnBackgroundErase, this);
	Bind(wxEVT_THREAD, &wxDisplayPanel::OnImageUpdate, this);

	m_widthScaleFactor = 0;
	m_heightScaleFactor = 0;
	m_frameCount = 0;
	m_displayRefreshRate = false;
	ResetNESPicture(m_picture);
}

void wxDisplayPanel::Render(wxPaintEvent& evt) {
	wxBufferedPaintDC dc(this);

	wxImage* image = new wxImage(NES_PICTURE_WIDTH, NES_PICTURE_HEIGHT);
	NESPicture* picture = new NESPicture(this->GetImage());
	for (unsigned int y = 0; y < NES_PICTURE_HEIGHT; y++) {
		for (unsigned int x = 0; x < NES_PICTURE_WIDTH; x++) {
			image->SetRGB(x, y, (* picture)[y][x].red, (*picture)[y][x].green, (*picture)[y][x].blue);
		}
	}
	delete picture;
	picture = nullptr;

	dc.SetUserScale(m_widthScaleFactor, m_heightScaleFactor);
	if ((*image).IsOk()) {
		dc.DrawBitmap(*image, 0, 0, false);
	}
	delete image;
	image = nullptr;

	if (m_displayRefreshRate) {
		m_frameCount++;
		std::chrono::duration<long long, std::nano> runtime = m_clock.now() - m_startTime;
		double seconds = runtime.count() / 1000000000.0;
		double refreshRate = m_frameCount / seconds;
		
		int width = this->GetClientSize().GetWidth();
		dc.SetFont(wxFont(wxFontInfo(width / 20)));
		dc.SetBackgroundMode(wxTRANSPARENT);
		dc.SetTextForeground(*wxBLACK);
		dc.DrawText(wxString::Format("%.2f", refreshRate), wxPoint(0, 0));
		dc.DrawText(wxString::Format("%.2f", refreshRate), wxPoint(0, 2));
		dc.DrawText(wxString::Format("%.2f", refreshRate), wxPoint(2, 0));
		dc.DrawText(wxString::Format("%.2f", refreshRate), wxPoint(2, 2));
		dc.SetTextForeground(*wxYELLOW);
		dc.DrawText(wxString::Format("%.2f", refreshRate), wxPoint(1, 1));
	}

	evt.Skip();
}

void wxDisplayPanel::OnSize(wxSizeEvent& evt) {
	wxSize currSize = evt.GetSize();
	m_widthScaleFactor = (double)currSize.GetWidth() / (double)NES_PICTURE_WIDTH;
	m_heightScaleFactor = (double)currSize.GetHeight() / (double)NES_PICTURE_HEIGHT;
	this->Refresh();
	evt.Skip();
}

void wxDisplayPanel::OnImageUpdate(wxThreadEvent& evt) {
	this->Refresh();
}

void wxDisplayPanel::OnBackgroundErase(wxEraseEvent& evt) {}

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
	this->Refresh();
}