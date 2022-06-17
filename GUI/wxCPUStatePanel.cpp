#include "wxCPUStatePanel.h"
#include "../nes/NES.h"

wxDEFINE_EVENT(EVT_CPU_STATE_GUI_UPDATE, wxNESStateEvent<CPUState>);
wxBEGIN_EVENT_TABLE(wxCPUStatePanel, wxPanel)
	EVT_SIZE(wxCPUStatePanel::OnResize)
wxEND_EVENT_TABLE()

wxCPUStatePanel::wxCPUStatePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	Bind(EVT_CPU_STATE_GUI_UPDATE, &wxCPUStatePanel::OnGUIUpdate, this);

	const int labelToValBorder = 20;
	const int inbetweenRegBorder = 40;
	const int rowBorder = 10;
	const int flagsRowBorder = 20;
	const int flagsBorder = 10;
	const int otherInfoBorder = 25;
	const int otherInfoLabelToValBorder = 15;

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* topSizer2 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* innerFrameSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* regAXSizer = new wxBoxSizer(wxHORIZONTAL);
	m_regAName = new wxStaticText(this, wxID_ANY, wxString("A :"));
	m_regAVal = new wxStaticText(this, wxID_ANY, wxString("0x00"));
	m_regXName = new wxStaticText(this, wxID_ANY, wxString("X :"));
	m_regXVal = new wxStaticText(this, wxID_ANY, wxString("0x00"));
	regAXSizer->Add(m_regAName);
	regAXSizer->Add(m_regAVal, 0, wxLEFT, labelToValBorder);
	regAXSizer->Add(m_regXName, 0, wxLEFT, inbetweenRegBorder);
	regAXSizer->Add(m_regXVal, 0, wxLEFT, labelToValBorder);

	wxBoxSizer* regYSSizer = new wxBoxSizer(wxHORIZONTAL);
	m_regYName = new wxStaticText(this, wxID_ANY, wxString("Y :"));
	m_regYVal = new wxStaticText(this, wxID_ANY, wxString("0x00"));
	m_regSName = new wxStaticText(this, wxID_ANY, wxString("S :"));
	m_regSVal = new wxStaticText(this, wxID_ANY, wxString("0x00"));
	regYSSizer->Add(m_regYName);
	regYSSizer->Add(m_regYVal, 0, wxLEFT, labelToValBorder);
	regYSSizer->Add(m_regSName, 0, wxLEFT, inbetweenRegBorder);
	regYSSizer->Add(m_regSVal, 0, wxLEFT, labelToValBorder);

	wxBoxSizer* regPCPSizer = new wxBoxSizer(wxHORIZONTAL);
	m_regPCName = new wxStaticText(this, wxID_ANY, wxString("PC :"));
	m_regPCVal = new wxStaticText(this, wxID_ANY, wxString("0x0000"));
	m_regPName = new wxStaticText(this, wxID_ANY, wxString("P :"));
	m_regPVal = new wxStaticText(this, wxID_ANY, wxString("0x00"));
	regPCPSizer->Add(m_regPCName);
	regPCPSizer->Add(m_regPCVal, 0, wxLEFT, labelToValBorder);
	regPCPSizer->Add(m_regPName, 0, wxLEFT, inbetweenRegBorder);
	regPCPSizer->Add(m_regPVal, 0, wxLEFT, labelToValBorder);

	wxBoxSizer* flagsSizer = new wxBoxSizer(wxHORIZONTAL);
	
	wxBoxSizer* nFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagNName = new wxStaticText(this, wxID_ANY, wxString("N"));
	m_flagNVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	nFlagSizer->Add(m_flagNName, 0, wxALIGN_CENTER);
	nFlagSizer->Add(m_flagNVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	wxBoxSizer* vFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagVName = new wxStaticText(this, wxID_ANY, wxString("V"));
	m_flagVVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	vFlagSizer->Add(m_flagVName, 0, wxALIGN_CENTER);
	vFlagSizer->Add(m_flagVVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	wxBoxSizer* uFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagUName = new wxStaticText(this, wxID_ANY, wxString("U"));
	m_flagUVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	uFlagSizer->Add(m_flagUName, 0, wxALIGN_CENTER);
	uFlagSizer->Add(m_flagUVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	wxBoxSizer* bFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagBName = new wxStaticText(this, wxID_ANY, wxString("B"));
	m_flagBVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	bFlagSizer->Add(m_flagBName, 0, wxALIGN_CENTER);
	bFlagSizer->Add(m_flagBVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	wxBoxSizer* dFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagDName = new wxStaticText(this, wxID_ANY, wxString("D"));
	m_flagDVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	dFlagSizer->Add(m_flagDName, 0, wxALIGN_CENTER);
	dFlagSizer->Add(m_flagDVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	wxBoxSizer* iFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagIName = new wxStaticText(this, wxID_ANY, wxString("I"));
	m_flagIVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	iFlagSizer->Add(m_flagIName, 0, wxALIGN_CENTER);
	iFlagSizer->Add(m_flagIVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	wxBoxSizer* zFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagZName = new wxStaticText(this, wxID_ANY, wxString("Z"));
	m_flagZVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	zFlagSizer->Add(m_flagZName, 0, wxALIGN_CENTER);
	zFlagSizer->Add(m_flagZVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	wxBoxSizer* cFlagSizer = new wxBoxSizer(wxVERTICAL);
	m_flagCName = new wxStaticText(this, wxID_ANY, wxString("C"));
	m_flagCVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	cFlagSizer->Add(m_flagCName, 0, wxALIGN_CENTER);
	cFlagSizer->Add(m_flagCVal, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	flagsSizer->Add(nFlagSizer);
	flagsSizer->Add(vFlagSizer, 0, wxLEFT, flagsBorder);
	flagsSizer->Add(uFlagSizer, 0, wxLEFT, flagsBorder);
	flagsSizer->Add(bFlagSizer, 0, wxLEFT, flagsBorder);
	flagsSizer->Add(dFlagSizer, 0, wxLEFT, flagsBorder);
	flagsSizer->Add(iFlagSizer, 0, wxLEFT, flagsBorder);
	flagsSizer->Add(zFlagSizer, 0, wxLEFT, flagsBorder);
	flagsSizer->Add(cFlagSizer, 0, wxLEFT, flagsBorder);


	wxBoxSizer* otherInfoSizer = new wxBoxSizer(wxHORIZONTAL);
	m_cyclesName = new wxStaticText(this, wxID_ANY, wxString("Cycles:"));
	m_cyclesVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	m_irqPendingName = new wxStaticText(this, wxID_ANY, wxString("IRQ:"));
	m_irqPendingVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	m_nmiRaisedName = new wxStaticText(this, wxID_ANY, wxString("NMI:"));
	m_nmiRaisedVal = new wxStaticText(this, wxID_ANY, wxString("0"));
	otherInfoSizer->Add(m_cyclesName, 0, wxLEFT, otherInfoLabelToValBorder);
	otherInfoSizer->Add(m_cyclesVal, 0, wxLEFT, otherInfoLabelToValBorder);
	otherInfoSizer->Add(m_irqPendingName, 0, wxLEFT, otherInfoLabelToValBorder);
	otherInfoSizer->Add(m_irqPendingVal, 0, wxLEFT, otherInfoLabelToValBorder);
	otherInfoSizer->Add(m_nmiRaisedName, 0, wxLEFT, otherInfoLabelToValBorder);
	otherInfoSizer->Add(m_nmiRaisedVal, 0, wxLEFT, otherInfoLabelToValBorder);


	innerFrameSizer->Add(regAXSizer, 0, wxALIGN_CENTER);
	innerFrameSizer->Add(regYSSizer, 0, wxTOP | wxALIGN_CENTER, rowBorder);
	innerFrameSizer->Add(regPCPSizer, 0, wxTOP | wxALIGN_CENTER, rowBorder);
	innerFrameSizer->Add(flagsSizer, 0, wxTOP | wxALIGN_CENTER, flagsRowBorder);
	innerFrameSizer->Add(otherInfoSizer, 0, wxTOP | wxALIGN_CENTER, otherInfoBorder);

	topSizer2->Add(innerFrameSizer, 1, wxALIGN_CENTER);
	topSizer->Add(topSizer2, 1, wxALIGN_CENTER);
	this->SetSizer(topSizer);
}

void wxCPUStatePanel::OnResize(wxSizeEvent& evt) {
	int newHeight = evt.GetSize().GetHeight();
	int newWidth = evt.GetSize().GetWidth();
	int fontSize = std::min(newHeight / 20, newWidth / 20);


	m_regAName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regAVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regXName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regXVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regYName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regYVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regSName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regSVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regPCName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regPCVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regPName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_regPVal->SetFont(wxFont(wxFontInfo(fontSize)));

	m_flagNName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagVName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagUName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagBName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagDName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagIName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagZName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagCName->SetFont(wxFont(wxFontInfo(fontSize)));

	m_flagNVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagVVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagUVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagBVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagDVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagIVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagZVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_flagCVal->SetFont(wxFont(wxFontInfo(fontSize)));

	m_cyclesName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_cyclesVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_irqPendingName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_irqPendingVal->SetFont(wxFont(wxFontInfo(fontSize)));
	m_nmiRaisedName->SetFont(wxFont(wxFontInfo(fontSize)));
	m_nmiRaisedVal->SetFont(wxFont(wxFontInfo(fontSize)));

	evt.Skip();
}

void wxCPUStatePanel::OnGUIUpdate(wxNESStateEvent<CPUState>& evt) {
	CPUState state = evt.GetState();
	m_regAVal->SetLabel(wxString::Format(wxT("0x%02X"), state.regA));
	m_regXVal->SetLabel(wxString::Format(wxT("0x%02X"), state.regX));
	m_regYVal->SetLabel(wxString::Format(wxT("0x%02X"), state.regY));
	m_regSVal->SetLabel(wxString::Format(wxT("0x%02X"), state.regS));
	m_regPCVal->SetLabel(wxString::Format(wxT("0x%04X"), state.regPC));
	m_regPVal->SetLabel(wxString::Format(wxT("0x%02X"), state.regP.value));

	m_flagNVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.N));
	m_flagVVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.V));
	m_flagUVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.Unused));
	m_flagBVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.B));
	m_flagDVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.D));
	m_flagIVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.I));
	m_flagZVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.Z));
	m_flagCVal->SetLabel(wxString::Format(wxT("%d"), state.regP.flags.C));

	m_cyclesVal->SetLabel(wxString::Format(wxT("%d"), state.cyclesRemaining));
	m_irqPendingVal->SetLabel(wxString::Format(wxT("%d"), state.irqPending));
	m_nmiRaisedVal->SetLabel(wxString::Format(wxT("%d"), state.nmiRaised));
}