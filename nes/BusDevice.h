/*
 * BusDevice.h
 *
 * Abstract class for a device connected to CPU or PPU bus
 * Should implement a read and write from bus functions
 * Relevant address space to be set by C'tor
 *
 */

#pragma once

#include <cstdint>
#include <utility>
#include <list>
#include <memory>

// Need to declare bus here to avoid circular dependency
// between headers
class Bus;


// Address range is in the form (LowAddr, HighAddr) and covers
// all address the device is interested in
// Ranges are inclusive, meaning the following addresses are covered:
// range.first() <= address <= range.second()
typedef std::pair<uint16_t, uint16_t> AddressRange;


class BusDevice {
public:
	BusDevice(std::list<AddressRange> addressRanges);
	virtual ~BusDevice() = default;
	BusDevice(const BusDevice& device) = delete;
	BusDevice& operator=(const BusDevice& device) = delete;

	virtual void SoftReset() = 0;
	virtual void HardReset() = 0;

	virtual uint8_t Read(uint16_t address) = 0;
	virtual void Write(uint16_t address, uint8_t data) = 0;

	virtual uint8_t Probe(uint16_t address) = 0;

	void ConnectToBus(Bus* bus);
	void DisconnectFromBus();
	bool IsInAddressRanges(uint16_t address);
	bool DoAddressesCollide(BusDevice* otherDevice);

protected:
	Bus* m_bus;

private:
	// A list of ranges so we can handle non-contiguous address ranges
	std::list<AddressRange> m_addressRanges;

};
