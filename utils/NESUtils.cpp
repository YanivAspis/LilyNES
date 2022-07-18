#include <sstream>
#include <iomanip>

#include "NESUtils.h"

namespace NESUtils {
	std::string HexUint8ToString(uint8_t num) {
		std::stringstream result;
		result << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (unsigned int)num;
		return result.str();
	}

	std::string DecUint8ToString(uint8_t num) {
		std::stringstream result;
		result << std::setfill('0') << std::setw(3) << (unsigned int)num;
		return result.str();
	}

	std::string HexUint16ToString(uint16_t num) {
		std::stringstream result;
		result << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << (unsigned int)num;
		return result.str();
	}
}
