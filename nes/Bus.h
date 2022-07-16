#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include "BusDevice.h"

class Bus {
public:
	Bus();
	~Bus();
	Bus(const Bus& bus) = delete;
	Bus& operator=(const Bus& bus) = delete;

	void SoftReset();
	void HardReset();

	uint8_t Read(uint16_t address);
	void Write(uint16_t address, uint8_t data);

	uint8_t Probe(uint16_t address);

	void ConnectDevice(BusDevice* device);
	void DisconnectDevice(BusDevice* device);
	void DisconnectAllDevices();
private:
	bool DoAddressesCollide(BusDevice* device);

	std::list<BusDevice*> m_devices;
	uint8_t m_lastReadValue; // For open bus behaviour
};
