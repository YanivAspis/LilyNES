#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include "wxNESStateEvent.h"
#include "../nes/RAMDevice.h"

wxDECLARE_EVENT(EVT_RAM_STATE_GUI_UPDATE, wxNESStateEvent<RAMState>);

class wxRAMStatePanel : public wxPanel {
public:
	wxRAMStatePanel(wxWindow* parent);

	void OnResize(wxSizeEvent& evt);
	void OnGUIUpdate(wxNESStateEvent<RAMState>& evt);

private:
	wxGrid* m_RAMViewGrid;

	wxDECLARE_EVENT_TABLE();
};
