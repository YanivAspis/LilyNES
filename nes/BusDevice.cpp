/*
 * BusDevice.cpp
 *
 *  Created on: 30 Jan 2022
 *      Author: yaniv
 */

#include "BusDevice.h"
#include <assert.h>

BusDevice::BusDevice(std::list<AddressRange> addressRanges) {
	m_bus = nullptr;
	m_addressRanges = std::list<AddressRange>(addressRanges);
}

void BusDevice::ConnectToBus(Bus* bus) {
	assert(m_bus == nullptr);
	m_bus = bus;
}

void BusDevice::DisconnectFromBus() {
	m_bus = nullptr;
}

bool BusDevice::IsInAddressRanges(uint16_t address) {
	for (AddressRange range : m_addressRanges) {
		if (address >= range.first && address <= range.second) {
			return true;
		}
	}
	return false;
}

bool BusDevice::DoAddressesCollide(BusDevice* otherDevice)
{
	for (AddressRange range : m_addressRanges) {
		if (otherDevice->IsInAddressRanges(range.first) || otherDevice->IsInAddressRanges(range.second)) {
			return true;
		}
	}
	for (AddressRange range : otherDevice->m_addressRanges) {
		if (this->IsInAddressRanges(range.first) || this->IsInAddressRanges(range.second)) {
			return true;
		}
	}
	return false;
}
