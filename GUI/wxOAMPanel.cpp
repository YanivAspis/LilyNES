#include "wxOAMPanel.h"

wxDEFINE_EVENT(EVT_OAM_STATE_GUI_UPDATE, wxNESStateEvent<OAMState>);
wxBEGIN_EVENT_TABLE(wxOAMPanel, wxPanel)
	EVT_SIZE(wxOAMPanel::OnSize)
wxEND_EVENT_TABLE()


wxOAMPanel::wxOAMPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	Bind(EVT_OAM_STATE_GUI_UPDATE, &wxOAMPanel::OnGUIUpdate, this);

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	m_OAMViewGrid = new wxGrid(this, wxID_ANY);
	m_OAMViewGrid->CreateGrid(OAM_NUM_SPRITES, 4);
	topSizer->Add(m_OAMViewGrid, 1, wxEXPAND | wxALL | wxFIXED_MINSIZE);

	m_OAMViewGrid->SetColLabelValue(0, wxString("y"));
	m_OAMViewGrid->SetColLabelValue(1, wxString("TileID"));
	m_OAMViewGrid->SetColLabelValue(2, wxString("Attribute"));
	m_OAMViewGrid->SetColLabelValue(3, wxString("x"));

	for (int i = 0; i < OAM_NUM_SPRITES; i++) {
		m_OAMViewGrid->SetRowLabelValue(i, wxString(wxString::Format(wxT("%d"), i)));
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < OAM_NUM_SPRITES; j++) {
			m_OAMViewGrid->SetCellAlignment(j, i, wxALIGN_CENTER, wxALIGN_CENTER);
			m_OAMViewGrid->SetCellValue(j, i, wxString("0x00"));
		}
	}

	m_OAMViewGrid->EnableEditing(false);
	m_OAMViewGrid->EnableGridLines(false);
	m_OAMViewGrid->DisableDragColMove();
	m_OAMViewGrid->DisableDragRowSize();
	m_OAMViewGrid->DisableDragColSize();
	m_OAMViewGrid->DisableDragGridSize();

	this->SetSizer(topSizer);
	m_OAMViewGrid->AutoSize();
}

void wxOAMPanel::OnSize(wxSizeEvent& evt) {
	int newHeight = evt.GetSize().GetHeight();
	int newWidth = evt.GetSize().GetWidth();
	int fontsize = std::min(newHeight / 48, newWidth / 48);
	m_OAMViewGrid->SetLabelFont(wxFont(wxFontInfo(fontsize)));
	m_OAMViewGrid->SetFont(wxFont(wxFontInfo(fontsize)));
	m_OAMViewGrid->SetDefaultCellFont(wxFont(wxFontInfo(fontsize)));
	m_OAMViewGrid->AutoSize();
	evt.Skip();
}

void wxOAMPanel::OnGUIUpdate(wxNESStateEvent<OAMState>& evt) {
	OAMState state = evt.GetState();
	for (int i = 0; i < OAM_NUM_SPRITES; i++) {
		for (int j = 0; j < 4; j++) {
			uint8_t value = 0;
			OAMEntry entry = state.entries[i];
			switch (j) {
			case 0:
				value = entry.y;
				break;
			case 1:
				value = entry.tileID;
				break;
			case 2:
				value = entry.attribute.value;
				break;
			case 3:
				value = entry.x;
				break;
			}
			m_OAMViewGrid->SetCellValue(i, j, wxString::Format(wxT("0x%02X"), value));
		}
	}
}