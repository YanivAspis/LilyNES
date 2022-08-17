#include "FrameCounterController2Device.h"

FrameCounterController2Device::FrameCounterController2Device(APU2A03* apu, ControllerDevice* controllers) : BusDevice(std::list<AddressRange>({AddressRange(FRAME_COUNTER_CONTROLLER_2_ADDRESS, FRAME_COUNTER_CONTROLLER_2_ADDRESS)}))
{
	m_apu = apu;
	m_controllers = controllers;
}

FrameCounterController2Device::~FrameCounterController2Device()
{
	m_apu = nullptr;
	m_controllers = nullptr;
}

void FrameCounterController2Device::SoftReset() {}

void FrameCounterController2Device::HardReset() {}

void FrameCounterController2Device::Read(uint16_t address, uint8_t& data)
{
	m_controllers->Read(address, data);
}

void FrameCounterController2Device::Write(uint16_t address, uint8_t data)
{
	m_apu->Write(address, data);
}

uint8_t FrameCounterController2Device::Probe(uint16_t address)
{
	return m_controllers->Probe(address);
}
