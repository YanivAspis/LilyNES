#include "wxROMInfoFrame.h"
#include "../nes/ROM/INESFile.h"


wxBEGIN_EVENT_TABLE(wxROMInfoFrame, wxFrame)
	EVT_SHOW(wxROMInfoFrame::OnShow)
wxEND_EVENT_TABLE()

wxROMInfoFrame::wxROMInfoFrame(wxMainFrame* parent) : wxFrame(parent, wxID_ANY, "ROM Information") {
	m_parent = parent;

	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* fileFormatSizer = new wxBoxSizer(wxHORIZONTAL);
	m_fileFormatName = new wxStaticText(this, wxID_ANY, "File Format: ");
	m_fileFormatVal = new wxStaticText(this, wxID_ANY, "iNES");
	fileFormatSizer->Add(m_fileFormatName, 0, wxLEFT);
	fileFormatSizer->Add(m_fileFormatVal, 0, wxLEFT);

	wxBoxSizer* mapperIDSizer = new wxBoxSizer(wxHORIZONTAL);
	m_mapperIDName = new wxStaticText(this, wxID_ANY, "Mapper: ");
	m_mapperIDVal = new wxStaticText(this, wxID_ANY, "000");
	mapperIDSizer->Add(m_mapperIDName, 0, wxLEFT);
	mapperIDSizer->Add(m_mapperIDVal, 0, wxLEFT);

	wxBoxSizer* PRGROMBankSizer = new wxBoxSizer(wxHORIZONTAL);
	m_PRGROMBanksName = new wxStaticText(this, wxID_ANY, "PRG ROM Banks: ");
	m_PRGROMBanksVal = new wxStaticText(this, wxID_ANY, "1 (16 KB)");
	PRGROMBankSizer->Add(m_PRGROMBanksName, 0, wxLEFT);
	PRGROMBankSizer->Add(m_PRGROMBanksVal, 0, wxLEFT);

	wxBoxSizer* CHRROMBankSizer = new wxBoxSizer(wxHORIZONTAL);
	m_CHRROMBanksName = new wxStaticText(this, wxID_ANY, "CHR ROM Banks: ");
	m_CHRROMBanksVal = new wxStaticText(this, wxID_ANY, "1 (8 KB)");
	CHRROMBankSizer->Add(m_CHRROMBanksName, 0, wxLEFT);
	CHRROMBankSizer->Add(m_CHRROMBanksVal, 0, wxLEFT);

	wxBoxSizer* PRGRAMSizeSizer = new wxBoxSizer(wxHORIZONTAL);
	m_PRGRAMSizeName = new wxStaticText(this, wxID_ANY, "PRG RAM Size: ");
	m_PRGRAMSizeVal = new wxStaticText(this, wxID_ANY, "8 KB");
	PRGRAMSizeSizer->Add(m_PRGRAMSizeName, 0, wxLEFT);
	PRGRAMSizeSizer->Add(m_PRGRAMSizeVal, 0, wxLEFT);

	wxBoxSizer* usesCHRRAMSizer = new wxBoxSizer(wxHORIZONTAL);
	m_usesCHRRAMName = new wxStaticText(this, wxID_ANY, "CHR RAM: ");
	m_usesCHRRAMVal = new wxStaticText(this, wxID_ANY, "No");
	usesCHRRAMSizer->Add(m_usesCHRRAMName, 0, wxLEFT);
	usesCHRRAMSizer->Add(m_usesCHRRAMVal, 0, wxLEFT);

	wxBoxSizer* fixedMirrorningSizer = new wxBoxSizer(wxHORIZONTAL);
	m_fixedMirroringName = new wxStaticText(this, wxID_ANY, "Fixed Mirroring Mode: ");
	m_fixedMirroringVal = new wxStaticText(this, wxID_ANY, "Horizontal");
	fixedMirrorningSizer->Add(m_fixedMirroringName, 0, wxLEFT);
	fixedMirrorningSizer->Add(m_fixedMirroringVal, 0, wxLEFT);

	wxBoxSizer* batteryBackedRAMSizer = new wxBoxSizer(wxHORIZONTAL);
	m_batteryBackedRAMName = new wxStaticText(this, wxID_ANY, "Battery Backed RAM: ");
	m_batteryBackedRAMVal = new wxStaticText(this, wxID_ANY, "No");
	batteryBackedRAMSizer->Add(m_batteryBackedRAMName, 0, wxLEFT);
	batteryBackedRAMSizer->Add(m_batteryBackedRAMVal, 0, wxLEFT);

	wxBoxSizer* fourScreenVRAMSizer = new wxBoxSizer(wxHORIZONTAL);
	m_fourScreenVRAMName = new wxStaticText(this, wxID_ANY, "Four Screen VRAM: ");
	m_fourScreenVRAMVal = new wxStaticText(this, wxID_ANY, "No");
	fourScreenVRAMSizer->Add(m_fourScreenVRAMName, 0, wxLEFT);
	fourScreenVRAMSizer->Add(m_fourScreenVRAMVal, 0, wxLEFT);

	topSizer->Add(fileFormatSizer);
	topSizer->Add(mapperIDSizer);
	topSizer->Add(PRGROMBankSizer);
	topSizer->Add(CHRROMBankSizer);
	topSizer->Add(PRGRAMSizeSizer);
	topSizer->Add(usesCHRRAMSizer);
	topSizer->Add(fixedMirrorningSizer);
	topSizer->Add(batteryBackedRAMSizer);
	topSizer->Add(fourScreenVRAMSizer);

	this->SetSizer(topSizer);
}

void wxROMInfoFrame::OnShow(wxShowEvent& evt) {
	std::shared_ptr<INESFile> ROMFile = m_parent->GetLoadedROM();
	m_fileFormatVal->SetLabel(this->GetFileFormatLabel(ROMFile->GetHeader().GetFileFormat()));
	m_mapperIDVal->SetLabel(this->GetMapperIDLabel(ROMFile->GetHeader().GetMapperId()));
	m_PRGROMBanksVal->SetLabel(this->GetPRGROMBanksLabel(ROMFile->GetHeader().GetNumPRGRomBanks()));
	m_CHRROMBanksVal->SetLabel(this->GetCHRROMBanksLabel(ROMFile->GetHeader().GetNumCHRRomBanks()));
	m_PRGRAMSizeVal->SetLabel(this->GetPRGRAMSizeLabel(ROMFile->GetHeader().GetPRGRAMSize()));
	m_usesCHRRAMVal->SetLabel(this->GetUsesCHRRAMLabel(ROMFile->GetHeader().UsesCHRRam()));
	m_fixedMirroringVal->SetLabel(this->GetFixedMirroringLabel(ROMFile->GetHeader().GetFixedMirroringMode()));
	m_batteryBackedRAMVal->SetLabel(this->GetBatteryBackedRAMLabel(ROMFile->GetHeader().IsPRGRAMBatteryBacked()));
	m_fourScreenVRAMVal->SetLabel(this->GetFourScreenVRAMLabel(ROMFile->GetHeader().UsesFourScreenVRAM()));

}

wxString wxROMInfoFrame::GetFileFormatLabel(INESFileFormat fileFormat)
{
	switch (fileFormat) {
	case INES_FORMAT_ARCHAIC_INES:
		return wxString("Archaic iNES");
	case INES_FORMAT_INES:
		return wxString("iNES");
	}

	// Should not reach here
	return wxString("ERROR");
}

wxString wxROMInfoFrame::GetMapperIDLabel(uint8_t mapperID)
{
	return wxString::Format(wxT("%03d"),mapperID);
}

wxString wxROMInfoFrame::GetPRGROMBanksLabel(size_t PRGROMBanks)
{
	return wxString::Format(wxT("%d (%d KB)"), (unsigned int)PRGROMBanks, 16 * (unsigned int)PRGROMBanks);
}

wxString wxROMInfoFrame::GetCHRROMBanksLabel(size_t CHRROMBanks)
{
	return wxString::Format(wxT("%d (%d KB)"), (unsigned int)CHRROMBanks, 8 * (unsigned int)CHRROMBanks);
}

wxString wxROMInfoFrame::GetPRGRAMSizeLabel(size_t PRGRAMSize)
{
	return wxString::Format(wxT("%d KB"), 8 * (unsigned int)PRGRAMSize);
}

wxString wxROMInfoFrame::GetUsesCHRRAMLabel(bool usesCHRRAM)
{
	return usesCHRRAM ? wxString("Yes") : wxString("No");
}

wxString wxROMInfoFrame::GetFixedMirroringLabel(FixedMirroringMode fixedMirroringMode)
{
	switch (fixedMirroringMode) {
	case FIXED_MIRROR_HORIZONTAL:
		return wxString("Horizontal (Vertical arrangement)");
	case FIXED_MIRROR_VERTICAL:
		return wxString("Vertical (Horizontal arrangement)");
	}

	// Should not reach here
	return wxString("ERROR");
}

wxString wxROMInfoFrame::GetBatteryBackedRAMLabel(bool batteryBackedRAM)
{
	return batteryBackedRAM ? wxString("Yes") : wxString("No");
}

wxString wxROMInfoFrame::GetFourScreenVRAMLabel(bool fourScreenVRAM)
{
	return fourScreenVRAM ? wxString("Yes") : wxString("No");
}






