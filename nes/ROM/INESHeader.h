#pragma once

#include <array>
#include <exception>

// Sizes in bytes
constexpr size_t HEADER_SIZE=16;
constexpr size_t TRAINER_SIZE = 512;
constexpr size_t PRG_ROM_BANK_SIZE = 16384;
constexpr size_t PRG_RAM_BANK_SIZE = 8192;
constexpr size_t CHR_ROM_BANK_SIZE = 8192;
constexpr size_t CHR_RAM_BANK_SIZE = 8192;
constexpr size_t PROM_INST_DATA_SIZE = 8192;



/*
enum MirroringMode {
	MIRROR_FIXED_HORIZONTAL,
	MIRROR_FIXED_VERTICAL,
	MIRROR_SWITCHABLE_HORIZONTAL_VERTICAL
};*/

enum FixedMirroringMode {
	FIXED_MIRROR_HORIZONTAL,
	FIXED_MIRROR_VERTICAL
};

// My implementation will support Archaic INES and INES but not NES 2.0, since very few ROMs are actually in NES 2.0 format
enum INESFileFormat {
	INES_FORMAT_ARCHAIC_INES,
	INES_FORMAT_INES,
	INES_FORMAT_NES2
};

enum ConsoleType {
	CONSOLE_NES_FAMICOM,
	CONSOLE_VSUNISYSTEM,
	CONSOLE_PLAYCHOICE10,
	CONSOLE_EXTENDED
};

enum TVSystem {
	TV_SYSTEM_NTSC,
	TV_SYSTEM_PAL
};

class INESFileException : public std::exception {};

class BadINESFileException : public INESFileException {
public:
	const char* what() const throw () {
		return "ROM file not in iNES format or corrupt.";
	}
};

class UnsupportedNes2Exception : public INESFileException {
public:
	const char* what() const throw () {
		return "ROM in NES 2.0 format which is currently unsupported.";
	}
};

class UnsupportedNesTrainerException : public INESFileException {
public:
	const char* what() const throw () {
		return "ROM contains trainer which is an unsupported feature.";
	}
};

class UnsupportedNesConsoleTypeException : public INESFileException {
public:
	const char* what() const throw () {
		return "ROM is not standard NES/Famicom which is currently unsupported.";
	}
};

class INESHeader {
public:
	INESHeader() = default;
	INESHeader(std::array<uint8_t, HEADER_SIZE> headerContent, size_t fileSize);

	size_t GetNumPRGRomBanks() const;
	size_t GetNumCHRRomBanks() const;
	size_t GetPRGRAMSize() const;
	bool UsesCHRRam() const;

	INESFileFormat GetFileFormat() const;
	
	FixedMirroringMode GetFixedMirroringMode() const;
	bool IsPRGRAMBatteryBacked() const;
	bool IsTrainerPresent() const;
	bool UsesFourScreenVRAM() const;
	ConsoleType GetConsoleType() const;
	TVSystem GetTVSystem() const;

	unsigned int GetMapperId() const;


private:
	bool VerifyFileSignature(std::array<uint8_t, HEADER_SIZE> headerContent);
	void ExtractFixedMirroring(uint8_t offset6);
	void ExtractBatteryBackedRAM(uint8_t offset6);
	void ExtractTrainer(uint8_t offset6);
	void ExtractFourScreenVRAM(uint8_t offset6);
	void ExtractConsoleType(uint8_t offset7);
	void ExtractINESFormatAndSizes(std::array<uint8_t, HEADER_SIZE> headerContent, size_t fileSize);
	void ExtractMapper(uint8_t offset6, uint8_t offset7);
	void ExtractPRGRAMSize(uint8_t offset8);
	void ExtractTVSystem(uint8_t offset9);

	static const std::array<uint8_t, 4> s_fileSignature;

	// Each bank is 16 KB
	size_t m_numPRGRomBanks;

	// Each bank is 8 KB. If 0, then uses CHR-RAM instead, which I am going to assume is always 8KB in size
	size_t m_numCHRRomBanks;

	// In 8 KB units. Usually only 8 KB (equal to 1) but can be specified. If header specifies 0, assume 8 KB
	size_t m_PRGRAMSize;

	INESFileFormat m_fileFormat;

	FixedMirroringMode m_mirroringMode;
	bool m_batteryBackedRAM;
	bool m_trainerPresent; // Trainers won't be supported
	bool m_fourScreenVRAM;
	ConsoleType m_consoleType; // Everything other than NES will be rejected
	TVSystem m_tvSystem; // Not sure if any ROMs actually specify this, so I'll ignore it

	unsigned int m_mapperId;
};


