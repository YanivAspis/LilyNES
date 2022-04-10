#include "TestDevice.h"

TestDevice::TestDevice(std::list<AddressRange> addresses) : BusDevice(addresses) {}

void TestDevice::SoftReset() {}

void TestDevice::HardReset() {}

uint8_t TestDevice::Read(uint16_t address)
{
    return 0x4;
}

void TestDevice::Write(uint16_t address, uint8_t data)
{
}
