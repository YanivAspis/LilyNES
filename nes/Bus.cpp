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
	m_addressToDevice.fill(nullptr);
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

uint8_t Bus::Read(uint16_t address, bool overrideOpenBus) {
	BusDevice* device = m_addressToDevice[address];
	if (device != nullptr) {
		if (overrideOpenBus) {
			uint8_t data;
			device->Read(address, data);
			return data;
		}
		else {
			device->Read(address, m_lastReadValue);
		}
	}
	// This mimics open bus behaviour of real NES
	// i.e. Last value read returned
	return m_lastReadValue;
}

void Bus::Write(uint16_t address, uint8_t data) {
	BusDevice* device = m_addressToDevice[address];
	if (device != nullptr) {
		device->Write(address, data);
	}

	// Address not claimed by any device (open bus)
	// To my understanding, the write request is ignored
	return;
}

uint8_t Bus::Probe(uint16_t address) {
	BusDevice* device = m_addressToDevice[address];
	uint8_t data = 0;
	if (device != nullptr) {
		device->Read(address, data);
	}

	return data;
}

void Bus::ConnectDevice(BusDevice* device) {
	assert(!DoAddressesCollide(device));
	device->ConnectToBus(this);
	m_devices.push_back(device);
	for (const AddressRange& addressRange : device->GetAddressRanges()) {
		for (size_t address = addressRange.first; address <= addressRange.second; address++) {
			m_addressToDevice[address] = device;
		}
	}
}

void Bus::DisconnectDevice(BusDevice* device) {
	device->DisconnectFromBus();
	m_devices.remove(device);
	for (const AddressRange& addressRange : device->GetAddressRanges()) {
		for (uint16_t address = addressRange.first; address <= addressRange.second; address++) {
			m_addressToDevice[address] = nullptr;
		}
	}
}

void Bus::DisconnectAllDevices() {
	for (BusDevice* device : m_devices) {
		device->DisconnectFromBus();
	}
	m_devices.clear();
	m_addressToDevice.fill(nullptr);
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
