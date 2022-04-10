#pragma once
#include "BusDevice.h"

class TestDevice : public BusDevice {
public:
	TestDevice(std::list<AddressRange> addresses);

	void SoftReset();
	void HardReset();

	uint8_t Read(uint16_t address);
	void Write(uint16_t address, uint8_t data);
};

