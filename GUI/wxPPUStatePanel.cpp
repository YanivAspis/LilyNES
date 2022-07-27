#include "wxPPUStatePanel.h"

wxDEFINE_EVENT(EVT_PPU_STATE_GUI_UPDATE, wxNESStateEvent<PPUState>);
wxBEGIN_EVENT_TABLE(wxPPUStatePanel, wxPanel)
	EVT_SIZE(wxPPUStatePanel::OnResize)
wxEND_EVENT_TABLE()

wxPPUStatePanel::wxPPUStatePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
	Bind(EVT_PPU_STATE_GUI_UPDATE, &wxPPUStatePanel::OnGUIUpdate, this);

	const int labelToValBorder = 10;
	const int rowBorder = 10;
	const int inbetweenRegBorder = 30;

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* topSizer2 = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* innerFrameSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* scanlineDotSizer = new wxBoxSizer(wxHORIZONTAL);
	m_scanlineLabel = new wxStaticText(this, wxID_ANY, wxString("Scanline:"));
	m_scanlineValue = new wxStaticText(this, wxID_ANY, wxString("000"));
	m_dotLabel = new wxStaticText(this, wxID_ANY, wxString("Dot:"));
	m_dotValue = new wxStaticText(this, wxID_ANY, wxString("000"));
	scanlineDotSizer->Add(m_scanlineLabel, 0);
	scanlineDotSizer->Add(m_scanlineValue, 0, wxLEFT, labelToValBorder);
	scanlineDotSizer->Add(m_dotLabel, 0, wxLEFT, inbetweenRegBorder);
	scanlineDotSizer->Add(m_dotValue, 0, wxLEFT, labelToValBorder);

	wxFlexGridSizer* PPUCTRLMASKSizer = new wxFlexGridSizer(2, 2, labelToValBorder, rowBorder);
	m_PPUCTRLLabel = new wxStaticText(this, wxID_ANY, wxString("CTRL:"));
	m_PPUCTRLValue = new wxStaticText(this, wxID_ANY, wxString("0 0 0 0 0 0 0 0"));
	m_PPUMASKLabel = new wxStaticText(this, wxID_ANY, wxString("MASK:"));
	m_PPUMASKValue = new wxStaticText(this, wxID_ANY, wxString("0 0 0 0 0 0 0 0"));
	PPUCTRLMASKSizer->Add(m_PPUCTRLLabel);
	PPUCTRLMASKSizer->Add(m_PPUCTRLValue);
	PPUCTRLMASKSizer->Add(m_PPUMASKLabel);
	PPUCTRLMASKSizer->Add(m_PPUMASKValue);

	wxBoxSizer* STATUSLatchBufferOAMADDRSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer* STATUSBufferSizer = new wxFlexGridSizer(2, 2, labelToValBorder, rowBorder);
	wxFlexGridSizer* IOLatchOAMADDRSizer = new wxFlexGridSizer(2, 2, labelToValBorder, rowBorder);

	m_PPUSTATUSLabel = new wxStaticText(this, wxID_ANY, wxString("STATUS:"));
	m_PPUSTATUSValue = new wxStaticText(this, wxID_ANY, wxString("0 0 0"));
	m_PPUIOLatchLabel = new wxStaticText(this, wxID_ANY, wxString("IO Latch:"));
	m_PPUIOLatchValue = new wxStaticText(this, wxID_ANY, wxString("0x00"));

	m_PPUDATABufferLabel = new wxStaticText(this, wxID_ANY, wxString("DATA:"));
	m_PPUDATABufferValue = new wxStaticText(this, wxID_ANY, wxString("0x00"));
	m_OAMADDRLabel = new wxStaticText(this, wxID_ANY, wxString("OAMADDR:"));
	m_OAMADDRValue = new wxStaticText(this, wxID_ANY, wxString("0x00"));


	STATUSBufferSizer->Add(m_PPUSTATUSLabel);
	STATUSBufferSizer->Add(m_PPUSTATUSValue);
	STATUSBufferSizer->Add(m_PPUDATABufferLabel);
	STATUSBufferSizer->Add(m_PPUDATABufferValue);

	IOLatchOAMADDRSizer->Add(m_PPUIOLatchLabel);
	IOLatchOAMADDRSizer->Add(m_PPUIOLatchValue);
	IOLatchOAMADDRSizer->Add(m_OAMADDRLabel);
	IOLatchOAMADDRSizer->Add(m_OAMADDRValue);

	STATUSLatchBufferOAMADDRSizer->Add(STATUSBufferSizer, 0);
	STATUSLatchBufferOAMADDRSizer->Add(IOLatchOAMADDRSizer, 0, wxLEFT, inbetweenRegBorder);

	wxFlexGridSizer* RAMSizer = new wxFlexGridSizer(2, 2, labelToValBorder, rowBorder);
	m_TRAMRegisterLabel = new wxStaticText(this, wxID_ANY, wxString("t:"));
	m_TRAMRegisterValue = new wxStaticText(this, wxID_ANY, wxString("0 0 0 00 00 [0x0000]"));
	m_VRAMRegisterLabel = new wxStaticText(this, wxID_ANY, wxString("v:"));
	m_VRAMRegisterValue = new wxStaticText(this, wxID_ANY, wxString("0 0 0 00 00 [0x0000]"));
	RAMSizer->Add(m_TRAMRegisterLabel);
	RAMSizer->Add(m_TRAMRegisterValue);
	RAMSizer->Add(m_VRAMRegisterLabel);
	RAMSizer->Add(m_VRAMRegisterValue);

	wxBoxSizer* writeToggleFineXSizer = new wxBoxSizer(wxHORIZONTAL);
	m_loopyWriteToggleLabel = new wxStaticText(this, wxID_ANY, wxString("w:"));
	m_loopyWriteToggleValue = new wxStaticText(this, wxID_ANY, wxString("0"));
	m_fineXLabel = new wxStaticText(this, wxID_ANY, wxString("x:"));
	m_fineXValue = new wxStaticText(this, wxID_ANY, wxString("0"));
	writeToggleFineXSizer->Add(m_loopyWriteToggleLabel, 0);
	writeToggleFineXSizer->Add(m_loopyWriteToggleValue, 0, wxLEFT, labelToValBorder);
	writeToggleFineXSizer->Add(m_fineXLabel, 0, wxLEFT, inbetweenRegBorder);
	writeToggleFineXSizer->Add(m_fineXValue, 0, wxLEFT, labelToValBorder);

	innerFrameSizer->Add(scanlineDotSizer, 0, wxALIGN_CENTER);
	innerFrameSizer->Add(PPUCTRLMASKSizer, 0, wxTOP | wxALIGN_CENTER, rowBorder);
	innerFrameSizer->Add(STATUSLatchBufferOAMADDRSizer, 0, wxTOP | wxALIGN_CENTER, rowBorder);
	innerFrameSizer->Add(RAMSizer, 0, wxTOP | wxALIGN_CENTER, rowBorder);
	innerFrameSizer->Add(writeToggleFineXSizer, 0, wxTOP | wxALIGN_CENTER, rowBorder);

	topSizer2->Add(innerFrameSizer, 1, wxALIGN_CENTER);
	topSizer->Add(topSizer2, 1, wxALIGN_CENTER);
	this->SetSizer(topSizer);

}

void wxPPUStatePanel::OnResize(wxSizeEvent& evt) {
	int newHeight = evt.GetSize().GetHeight();
	int newWidth = evt.GetSize().GetWidth();
	int fontSize = std::min(newHeight / 20, newWidth / 20);

	m_scanlineLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_scanlineValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_dotLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_dotValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUCTRLLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUCTRLValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUMASKLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUMASKValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUSTATUSLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUSTATUSValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUIOLatchLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUIOLatchValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUDATABufferLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_PPUDATABufferValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_OAMADDRLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_OAMADDRValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_TRAMRegisterLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_TRAMRegisterValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_VRAMRegisterLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_VRAMRegisterValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_loopyWriteToggleLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_loopyWriteToggleValue->SetFont(wxFont(wxFontInfo(fontSize)));
	m_fineXLabel->SetFont(wxFont(wxFontInfo(fontSize)));
	m_fineXValue->SetFont(wxFont(wxFontInfo(fontSize)));

	evt.Skip();
}

void wxPPUStatePanel::OnGUIUpdate(wxNESStateEvent<PPUState>& evt) {
	PPUState state = evt.GetState();
	m_scanlineValue->SetLabel(IntToString(state.scanline, 3));
	m_dotValue->SetLabel(IntToString(state.dot, 3));
	m_PPUCTRLValue->SetLabel(this->PPUCTRLToString(state.PPUCTRL));
	m_PPUMASKValue->SetLabel(this->PPUMASKToString(state.PPUMASK));
	m_PPUSTATUSValue->SetLabel(this->PPUSTATUSToString(state.PPUSTATUS));
	m_PPUIOLatchValue->SetLabel(HexUint8ToString(state.ioLatchValue));
	m_OAMADDRValue->SetLabel(HexUint8ToString(state.OAMADDR));
	m_TRAMRegisterValue->SetLabel(this->LoopyRegisterToString(state.TRAMAddress));
	m_VRAMRegisterValue->SetLabel(this->LoopyRegisterToString(state.VRAMAddress));
	m_loopyWriteToggleValue->SetLabel(std::to_string(state.loopyWriteToggle));
	m_fineXValue->SetLabel(std::to_string(state.fineX));
}

std::string wxPPUStatePanel::PPUCTRLToString(PPUCTRLRegister PPUCTRL) {
	return std::to_string(PPUCTRL.flags.NMIEnabled) + " " +
		std::to_string(PPUCTRL.flags.PPUMaster) + " " +
		std::to_string(PPUCTRL.flags.spriteSize) + " " +
		std::to_string(PPUCTRL.flags.backgroundPatternTable) + " " +
		std::to_string(PPUCTRL.flags.spritePatternTable) + " " +
		std::to_string(PPUCTRL.flags.incrementMode) + " " +
		std::to_string(PPUCTRL.flags.nametableY) + " " +
		std::to_string(PPUCTRL.flags.nametableX);
}

std::string wxPPUStatePanel::PPUMASKToString(PPUMASKRegister PPUMASK) {
	return std::to_string(PPUMASK.flags.emphasizeBlue) + " " +
		std::to_string(PPUMASK.flags.emphasizeGreen) + " " +
		std::to_string(PPUMASK.flags.emphasizeRed) + " " +
		std::to_string(PPUMASK.flags.renderSprites) + " " +
		std::to_string(PPUMASK.flags.renderBackground) + " " +
		std::to_string(PPUMASK.flags.showLeftmostSprites) + " " +
		std::to_string(PPUMASK.flags.showLeftmostBackground) + " " +
		std::to_string(PPUMASK.flags.greyscaleMode);
}

std::string wxPPUStatePanel::PPUSTATUSToString(PPUSTATUSRegister PPUSTATUS) {
	return std::to_string(PPUSTATUS.flags.VBlank) + " " +
		std::to_string(PPUSTATUS.flags.sprite0Hit) + " " +
		std::to_string(PPUSTATUS.flags.spriteOverflow);
}

std::string wxPPUStatePanel::LoopyRegisterToString(LoopyRegister loopyReg) {
	return std::to_string(loopyReg.scrollFlags.fineY) + " " +
		std::to_string(loopyReg.scrollFlags.nametableY) + " " +
		std::to_string(loopyReg.scrollFlags.nametableX) + " " +
		IntToString(loopyReg.scrollFlags.coarseY, 5) + " " +
		IntToString(loopyReg.scrollFlags.coarseX, 5) + " [" +
		HexUint16ToString(loopyReg.address) + "]";
}