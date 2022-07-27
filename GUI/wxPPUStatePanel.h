#pragma once

#include "wx/wx.h"
#include "wxNESStateEvent.h"
#include "../nes/PPU/PPU2C02.h"

wxDECLARE_EVENT(EVT_PPU_STATE_GUI_UPDATE, wxNESStateEvent<PPUState>);

class wxPPUStatePanel : public wxPanel {
public:
	wxPPUStatePanel(wxWindow* parent);
	void OnResize(wxSizeEvent& evt);
	void OnGUIUpdate(wxNESStateEvent<PPUState>& evt);

private:
	std::string PPUCTRLToString(PPUCTRLRegister PPUCTRL);
	std::string PPUMASKToString(PPUMASKRegister PPUMASK);
	std::string PPUSTATUSToString(PPUSTATUSRegister PPUSTATUS);
	std::string LoopyRegisterToString(LoopyRegister loopyReg);

	wxStaticText* m_scanlineLabel;
	wxStaticText* m_scanlineValue;

	wxStaticText* m_dotLabel;
	wxStaticText* m_dotValue;

	wxStaticText* m_PPUCTRLLabel;
	wxStaticText* m_PPUCTRLValue;

	wxStaticText* m_PPUMASKLabel;
	wxStaticText* m_PPUMASKValue;

	wxStaticText* m_PPUSTATUSLabel;
	wxStaticText* m_PPUSTATUSValue;

	wxStaticText* m_PPUIOLatchLabel;
	wxStaticText* m_PPUIOLatchValue;

	wxStaticText* m_PPUDATABufferLabel;
	wxStaticText* m_PPUDATABufferValue;

	wxStaticText* m_OAMADDRLabel;
	wxStaticText* m_OAMADDRValue;

	wxStaticText* m_TRAMRegisterLabel;
	wxStaticText* m_TRAMRegisterValue;

	wxStaticText* m_VRAMRegisterLabel;
	wxStaticText* m_VRAMRegisterValue;

	wxStaticText* m_loopyWriteToggleLabel;
	wxStaticText* m_loopyWriteToggleValue;

	wxStaticText* m_fineXLabel;
	wxStaticText* m_fineXValue;

	wxDECLARE_EVENT_TABLE();
};
