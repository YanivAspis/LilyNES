#include <fstream>
#include "INESFile.h"
#include "../mappers/Mapper000.h"


INESFile::INESFile(std::string romFilePath) {
	std::vector<uint8_t> fileContent;
	this->readFile(romFilePath, fileContent);

	m_checksum = MD5::ComputeChecksum(fileContent);

	// Read header content
	std::array<uint8_t, HEADER_SIZE> headerContent;
	std::copy_n(fileContent.begin(), HEADER_SIZE, headerContent.begin());
	m_header = INESHeader(headerContent, fileContent.size());

	// We assume no trainer and no additional rom information
	this->loadPRGROM(fileContent);
	this->loadCHRROM(fileContent);
}

std::array<uint8_t, MD5::CHECKSUM_SIZE> INESFile::GetChecksum() const {
	return m_checksum;
}


INESHeader INESFile::GetHeader() const {
	return m_header;
}

std::vector<uint8_t> INESFile::GetPRGROM() const {
	return m_PRGROMContent;
}

std::vector<uint8_t> INESFile::GetCHRROM() const {
	return m_CHRROMContent;
}

void INESFile::readFile(std::string romFilePath, std::vector<uint8_t>& fileContent) {
	std::ifstream fileStream(romFilePath, std::ios::binary);
	fileContent = std::vector<uint8_t>(std::istreambuf_iterator<char>(fileStream), {});
}

void INESFile::loadPRGROM(std::vector<uint8_t>& fileContent) {
	std::vector<uint8_t>::iterator PRGROMBegin = fileContent.begin() + HEADER_SIZE;
	std::vector<uint8_t>::iterator PRGROMEnd = PRGROMBegin + m_header.GetNumPRGRomBanks() * PRG_ROM_BANK_SIZE;
	m_PRGROMContent = std::vector<uint8_t>(PRGROMBegin, PRGROMEnd);
}

void INESFile::loadCHRROM(std::vector<uint8_t>& fileContent) {
	std::vector<uint8_t>::iterator CHRROMBegin = fileContent.begin() + HEADER_SIZE + m_header.GetNumPRGRomBanks() * PRG_ROM_BANK_SIZE;
	std::vector<uint8_t>::iterator CHRROMEnd = CHRROMBegin + m_header.GetNumCHRRomBanks() * CHR_ROM_BANK_SIZE;
	m_CHRROMContent = std::vector<uint8_t>(CHRROMBegin, CHRROMEnd);
}