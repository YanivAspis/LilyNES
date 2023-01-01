#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "BusDevice.h"
#include "../Environment.h"

class Environment;

constexpr uint16_t CONTROLLER_BEGIN_ADDRESS = 0x4016;
constexpr uint16_t CONTROLLER_END_ADDRESS = 0x4016;
constexpr uint16_t CONTROLLER_1_ADDRESS = CONTROLLER_BEGIN_ADDRESS;
constexpr uint16_t CONTROLLER_2_ADDRESS = 0x4017;
constexpr uint8_t CONTROLLER_INITIAL_VALUE = 0xFF;

enum NES_CONTROLLER_KEY {
	NES_KEY_A,
	NES_KEY_B,
	NES_KEY_SELECT,
	NES_KEY_START,
	NES_KEY_UP,
	NES_KEY_DOWN,
	NES_KEY_LEFT,
	NES_KEY_RIGHT,
};

enum NES_CONTROLLER_ID {
	NES_CONTROLLER_1,
	NES_CONTROLLER_2
};

struct ControllerState {
	ControllerState();
	uint8_t controller1Latch;
	uint8_t controller2Latch;
	bool latchFillToggle;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& controller1Latch;
		ar& controller2Latch;
		ar& latchFillToggle;
	}
};


class ControllerDevice : public BusDevice {
public:
	ControllerDevice(Environment* environment);

	void SoftReset() override;
	void HardReset() override;

	void Read(uint16_t address, uint8_t& data) override;
	void Write(uint16_t address, uint8_t data) override;

	uint8_t Probe(uint16_t address) override;

	ControllerState GetState() const;
	void LoadState(ControllerState& state);

private:
	void FillLatches();
	uint8_t GetLatchBit(NES_CONTROLLER_ID controller);

	Environment* m_environment;
	uint8_t m_controller1Latch;
	uint8_t m_controller2Latch;
	bool m_latchFillToggle; // When on, latches are begin filled (read key presses). Otherwise they remain the same until read
};
