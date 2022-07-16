#include "INESHeader.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;


// "NES" followed by MS-DOS eof. Should match the first 4 bytes of the header. 
const std::array<uint8_t, 4> INESHeader::s_fileSignature = { 0x4E, 0x45, 0x53, 0x1A };


INESHeader::INESHeader(std::array<uint8_t, HEADER_SIZE> headerContent, size_t fileSize)
{
	if (!this->VerifyFileSignature(headerContent)) {
		throw BadINESFileException();
	}
	this->ExtractINESFormatAndSizes(headerContent, fileSize);

	if (m_fileFormat == INES_FORMAT_NES2) {
		throw UnsupportedNes2Exception();
	}

	this->ExtractFixedMirroring(headerContent[6]);
	this->ExtractBatteryBackedRAM(headerContent[6]);
	this->ExtractTrainer(headerContent[6]);

	if (m_trainerPresent) {
		throw UnsupportedNesTrainerException();
	}

	this->ExtractFourScreenVRAM(headerContent[6]);
	
	if (m_fileFormat == INES_FORMAT_INES) {
		this->ExtractMapper(headerContent[6], headerContent[7]);
		this->ExtractConsoleType(headerContent[7]);
		if (m_consoleType != CONSOLE_NES_FAMICOM) {
			throw UnsupportedNesConsoleTypeException();
		}

		// Not often used, I might end up throwing an exception if offset 8 is not 0 or 1
		this->ExtractPRGRAMSize(headerContent[8]);

		// I don't support PAL, but I don't think many ROMs specify tv system anyways, so I'm ignoring this
		this->ExtractTVSystem(headerContent[9]);
	}
	else {
		// Archaic iNES. Only 16 mappers supported
		this->ExtractMapper(headerContent[6], 0x0);
		m_consoleType = CONSOLE_NES_FAMICOM;
		this->ExtractPRGRAMSize(0x0);

		// I don't think archaic iNES had any way of specifying PAL.
		m_tvSystem = TV_SYSTEM_NTSC;
	}
}

size_t INESHeader::GetNumPRGRomBanks() const
{
	return m_numPRGRomBanks;
}

size_t INESHeader::GetNumCHRRomBanks() const
{
	return m_numCHRRomBanks;
}

size_t INESHeader::GetPRGRAMSize() const
{
	return m_PRGRAMSize;
}

bool INESHeader::UsesCHRRam() const
{
	return m_numCHRRomBanks == 0;
}

INESFileFormat INESHeader::GetFileFormat() const
{
	return m_fileFormat;
}

FixedMirroringMode INESHeader::GetFixedMirroringMode() const
{
	return m_mirroringMode;
}

bool INESHeader::IsPRGRAMBatteryBacked() const
{
	return m_batteryBackedRAM;
}

bool INESHeader::IsTrainerPresent() const 
{
	return m_trainerPresent;
}

bool INESHeader::UsesFourScreenVRAM() const
{
	return m_fourScreenVRAM;
}

ConsoleType INESHeader::GetConsoleType() const
{
	return m_consoleType;
}

TVSystem INESHeader::GetTVSystem() const
{
	return m_tvSystem;
}

unsigned int INESHeader::GetMapperId() const
{
	return m_mapperId;
}


bool INESHeader::VerifyFileSignature(std::array<uint8_t, HEADER_SIZE> headerContent)
{
	for (size_t i = 0; i < 4; i++) {
		if (headerContent[i] != INESHeader::s_fileSignature[i]) {
			return false;
		}
	}
	return true;
}

void INESHeader::ExtractFixedMirroring(uint8_t offset6)
{
	m_mirroringMode = TestBit8(offset6, 0) ? FIXED_MIRROR_VERTICAL : FIXED_MIRROR_HORIZONTAL;
}

void INESHeader::ExtractBatteryBackedRAM(uint8_t offset6)
{
	m_batteryBackedRAM = TestBit8(offset6, 1) ? true : false;
}

void INESHeader::ExtractTrainer(uint8_t offset6)
{
	m_trainerPresent = TestBit8(offset6, 2) ? true : false;
}

void INESHeader::ExtractFourScreenVRAM(uint8_t offset6)
{
	m_fourScreenVRAM = TestBit8(offset6, 3) ? true : false;
}

void INESHeader::ExtractConsoleType(uint8_t offset7)
{
	uint8_t consoleTypeMask = ClearUpperBits8(offset7, 2);
	switch (consoleTypeMask) {
	case 0x00:
		m_consoleType = CONSOLE_NES_FAMICOM;
		break;
	case 0x01:
		m_consoleType = CONSOLE_VSUNISYSTEM;
		break;
	case 0x02:
		m_consoleType = CONSOLE_PLAYCHOICE10;
		break;
	case 0x03:
		m_consoleType = CONSOLE_EXTENDED;
		break;
	}
}

void INESHeader::ExtractINESFormatAndSizes(std::array<uint8_t, HEADER_SIZE> headerContent, size_t fileSize)
{
	m_numPRGRomBanks = headerContent[4];
	m_numCHRRomBanks = headerContent[5];

	if (TestBit8(headerContent[7], 4) && !TestBit8(headerContent[7], 3)) {
		// NES 2.0 flags are set, Check if actually NES 2.0
		uint8_t PRGRomMSB = ClearUpperBits8(headerContent[9], 4);
		uint8_t CHRRomMSB = ShiftRight8(ClearLowerBits8(headerContent[9], 4), 4);
		size_t totalPRGRomBanks = CombineBytes((uint8_t)m_numPRGRomBanks, PRGRomMSB);
		size_t totalCHRRomBanks = CombineBytes((uint8_t)m_numCHRRomBanks, CHRRomMSB);
		this->ExtractTrainer(headerContent[6]);
		this->ExtractConsoleType(headerContent[7]);

		// Compute the minimum size required (in bytes) according to NES2.0 format, and compare to actual file size
		size_t minSizeRequired = HEADER_SIZE + (m_trainerPresent ? TRAINER_SIZE : 0) + PRG_ROM_BANK_SIZE * totalPRGRomBanks + CHR_ROM_BANK_SIZE * totalCHRRomBanks;
		if (m_consoleType == CONSOLE_PLAYCHOICE10) {
			minSizeRequired += PROM_INST_DATA_SIZE;
		}

		if (minSizeRequired <= fileSize) {
			m_fileFormat = INES_FORMAT_NES2;
			return;
		}
	}

	// If here, either INES or Archaic INES, determine this by offset 12-16 (should be all zeros for INES)
	for (size_t offset = 12; offset < HEADER_SIZE; offset++) {
		if (headerContent[offset] != 0) {
			m_fileFormat = INES_FORMAT_ARCHAIC_INES;
			return;
		}
	}
	m_fileFormat = INES_FORMAT_INES;
}

void INESHeader::ExtractMapper(uint8_t offset6, uint8_t offset7)
{
	uint8_t mapperLSB = ShiftRight8(offset6, 4);
	uint8_t mapperMSB = ClearLowerBits8(offset7, 4);
	m_mapperId = mapperLSB | mapperMSB;
}

void INESHeader::ExtractPRGRAMSize(uint8_t offset8)
{
	if (offset8 == 0) {
		m_PRGRAMSize = 1;
	}
	else {
		m_PRGRAMSize = offset8;
	}
}

void INESHeader::ExtractTVSystem(uint8_t offset9)
{
	m_tvSystem = TestBit8(offset9, 0) ? TV_SYSTEM_PAL : TV_SYSTEM_NTSC;
}


