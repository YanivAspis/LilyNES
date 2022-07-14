#pragma once
#include "wx/wx.h"
#include "wxMainFrame.h"
#include "../nes/ROM/INESHeader.h"

class wxMainFrame;


class wxROMInfoFrame : public wxFrame {
public:
	wxROMInfoFrame(wxMainFrame* parent);
	void OnShow(wxShowEvent& evt);


private:
	wxString GetFileFormatLabel(INESFileFormat fileFormat);
	wxString GetMapperIDLabel(uint8_t mapperID);
	wxString GetPRGROMBanksLabel(size_t PRGROMBanks);
	wxString GetCHRROMBanksLabel(size_t CHRROMBanks);
	wxString GetPRGRAMSizeLabel(size_t PRGRAMSize);
	wxString GetUsesCHRRAMLabel(bool usesCHRRAM);
	wxString GetFixedMirroringLabel(FixedMirroringMode fixedMirroringMode);
	wxString GetBatteryBackedRAMLabel(bool batteryBackedRAM);
	wxString GetFourScreenVRAMLabel(bool fourScreenVRAM);

	wxMainFrame* m_parent;

	wxStaticText* m_fileFormatName;
	wxStaticText* m_fileFormatVal;

	wxStaticText* m_mapperIDName;
	wxStaticText* m_mapperIDVal;

	wxStaticText* m_PRGROMBanksName;
	wxStaticText* m_PRGROMBanksVal;

	wxStaticText* m_CHRROMBanksName;
	wxStaticText* m_CHRROMBanksVal;

	wxStaticText* m_PRGRAMSizeName;
	wxStaticText* m_PRGRAMSizeVal;

	wxStaticText* m_usesCHRRAMName;
	wxStaticText* m_usesCHRRAMVal;

	wxStaticText* m_fixedMirroringName;
	wxStaticText* m_fixedMirroringVal;

	wxStaticText* m_batteryBackedRAMName;
	wxStaticText* m_batteryBackedRAMVal;

	wxStaticText* m_fourScreenVRAMName;
	wxStaticText* m_fourScreenVRAMVal;

	DECLARE_EVENT_TABLE();
};
