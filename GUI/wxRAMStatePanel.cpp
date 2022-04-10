#include "wxRAMStatePanel.h"

wxDEFINE_EVENT(EVT_RAM_STATE_GUI_UPDATE, wxNESStateEvent<RAMState>);
wxBEGIN_EVENT_TABLE(wxRAMStatePanel, wxPanel)
	EVT_SIZE(wxRAMStatePanel::OnResize)
wxEND_EVENT_TABLE()

wxRAMStatePanel::wxRAMStatePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	Bind(EVT_RAM_STATE_GUI_UPDATE, &wxRAMStatePanel::OnGUIUpdate, this);

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	m_RAMViewGrid = new wxGrid(this, wxID_ANY);
	m_RAMViewGrid->CreateGrid(RAM_PHYSICAL_SIZE / 16, 16);
	topSizer->Add(m_RAMViewGrid, 1, wxEXPAND | wxALL);

	for (int i = 0; i < 16; i++) {
		m_RAMViewGrid->SetColLabelValue(i, wxString(wxString::Format(wxT("%X"), i)));
	}

	for (int i = 0; i < RAM_PHYSICAL_SIZE / 16; i++) {
		m_RAMViewGrid->SetRowLabelValue(i, wxString(wxString::Format(wxT("0x%04X"), i * 16)));
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < RAM_PHYSICAL_SIZE / 16; j++) {
			m_RAMViewGrid->SetCellAlignment(j, i, wxALIGN_CENTER, wxALIGN_CENTER);
			m_RAMViewGrid->SetCellValue(j, i, wxString("0x00"));
		}
	}

	m_RAMViewGrid->EnableEditing(false);
	m_RAMViewGrid->EnableGridLines(false);
	m_RAMViewGrid->DisableDragColMove();
	m_RAMViewGrid->DisableDragRowSize();
	m_RAMViewGrid->DisableDragColSize();
	m_RAMViewGrid->DisableDragGridSize();
	
	this->SetSizer(topSizer);
	m_RAMViewGrid->AutoSize();
}

void wxRAMStatePanel::OnResize(wxSizeEvent& evt) {
	int newHeight = evt.GetSize().GetHeight();
	int newWidth = evt.GetSize().GetWidth();
	int fontsize = std::min(newHeight / 48, newWidth / 48);
	m_RAMViewGrid->SetLabelFont(wxFont(wxFontInfo(fontsize)));
	m_RAMViewGrid->SetFont(wxFont(wxFontInfo(fontsize)));
	m_RAMViewGrid->SetDefaultCellFont(wxFont(wxFontInfo(fontsize)));
	m_RAMViewGrid->AutoSize();
	evt.Skip();
}

void wxRAMStatePanel::OnGUIUpdate(wxNESStateEvent<RAMState>& evt) {
	RAMState state = evt.GetState();
	for (int i = 0; i < RAM_PHYSICAL_SIZE / 16; i++) {
		for (int j = 0; j < 16; j++) {
			m_RAMViewGrid->SetCellValue(i, j, wxString::Format(wxT("0x%02X"), state.content[16 * i + j]));
		}
	}
}