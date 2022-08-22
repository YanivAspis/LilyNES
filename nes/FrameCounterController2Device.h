#pragma once

#include "BusDevice.h"
#include "APU/APU2A03.h"
#include "ControllerDevice.h"

constexpr uint16_t FRAME_COUNTER_CONTROLLER_2_ADDRESS = 0x4017;

class FrameCounterController2Device : public BusDevice {
public:
	FrameCounterController2Device(APU2A03* apu, ControllerDevice* controllers);
	~FrameCounterController2Device();

	void SoftReset() override;
	void HardReset() override;

	void Read(uint16_t address, uint8_t& data) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

private:
	APU2A03* m_apu;
	ControllerDevice* m_controllers;
};
