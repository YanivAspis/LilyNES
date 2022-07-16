/*
 * Bus.cpp
 *
 *  Created on: 30 Jan 2022
 *      Author: yaniv
 */

#include <assert.h>
#include "Bus.h"

Bus::Bus() {
	m_lastReadValue = 0;
}

Bus::~Bus() {
	this->DisconnectAllDevices();
}

void Bus::SoftReset()
{
	for (BusDevice* device : m_devices) {
		device->SoftReset();
	}
}

void Bus::HardReset()
{
	for (BusDevice* device : m_devices) {
		device->HardReset();
	}
}

uint8_t Bus::Read(uint16_t address) {
	for (BusDevice* device : m_devices) {
		if (device->IsInAddressRanges(address)) {
			m_lastReadValue = device->Read(address);
			return m_lastReadValue;
		}
	}

	// Address not claimed by any device (open bus)
	// Simply returning 0 is useful while developing,
	// but does not emulate some games correctly
	return 0;

	// This mimics open bus behaviour of real NES
	// i.e. Last value read returned
	//return m_lastReadValue;
}

void Bus::Write(uint16_t address, uint8_t data) {
	for (BusDevice* device : m_devices) {
		if (device->IsInAddressRanges(address)) {
			device->Write(address, data);
			return;
		}
	}

	// Address not claimed by any device (open bus)
	// To my understanding, the write request is ignored
	return;
}

uint8_t Bus::Probe(uint16_t address) {
	for (BusDevice* device : m_devices) {
		if (device->IsInAddressRanges(address)) {
			return device->Probe(address);;
		}
	}

	// Address not claimed by any device
	return 0;
}

void Bus::ConnectDevice(BusDevice* device) {
	assert(!DoAddressesCollide(device));
	device->ConnectToBus(this);
	m_devices.push_back(device);
}

void Bus::DisconnectDevice(BusDevice* device) {
	device->DisconnectFromBus();
	m_devices.remove(device);
}

void Bus::DisconnectAllDevices() {
	for (BusDevice* device : m_devices) {
		device->DisconnectFromBus();
	}
	m_devices.clear();
}

bool Bus::DoAddressesCollide(BusDevice* device)
{
	for (BusDevice* existingDevice : m_devices) {
		if (device->DoAddressesCollide(existingDevice)) {
			return true;
		}
	}
	return false;
}
