#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include "wxNESStateEvent.h"
#include "../nes/PPU/OAM.h"

wxDECLARE_EVENT(EVT_OAM_STATE_GUI_UPDATE, wxNESStateEvent<OAMState>);


class wxOAMPanel : public wxPanel {
public:
	wxOAMPanel(wxWindow* parent);
	void OnSize(wxSizeEvent& evt);
	void OnGUIUpdate(wxNESStateEvent<OAMState>& evt);

private:
	wxGrid* m_OAMViewGrid;

	wxDECLARE_EVENT_TABLE();
};
