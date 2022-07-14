#pragma once

#include <string>
#include <vector>
#include "INESHeader.h"


class INESFile {
public:
	INESFile(std::string romFilePath);

	INESHeader GetHeader() const;
	std::vector<uint8_t> GetPRGROM() const;
	std::vector<uint8_t> GetCHRROM() const;

private:
	void readFile(std::string romFilePath, std::vector<uint8_t>& fileContent);
	void loadPRGROM(std::vector<uint8_t>& fileContent);
	void loadCHRROM(std::vector<uint8_t>& fileContent);

	INESHeader m_header;
	std::vector<uint8_t> m_PRGROMContent;
	std::vector<uint8_t> m_CHRROMContent;
};