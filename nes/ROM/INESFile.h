#pragma once

#include <string>
#include <vector>
#include "INESHeader.h"
#include "../../utils/NESUtils.h"

using namespace NESUtils;

class INESFile {
public:
	INESFile(std::string romFilePath);

	std::array<uint8_t, MD5::CHECKSUM_SIZE> GetChecksum() const;
	INESHeader GetHeader() const;
	std::vector<uint8_t> GetPRGROM() const;
	std::vector<uint8_t> GetCHRROM() const;

private:
	void readFile(std::string romFilePath, std::vector<uint8_t>& fileContent);
	void loadPRGROM(std::vector<uint8_t>& fileContent);
	void loadCHRROM(std::vector<uint8_t>& fileContent);

	std::array<uint8_t, MD5::CHECKSUM_SIZE> m_checksum;
	INESHeader m_header;
	std::vector<uint8_t> m_PRGROMContent;
	std::vector<uint8_t> m_CHRROMContent;
};