#include "APU2A03.h"
#include "../../utils/BitwiseUtils.h"

using namespace BitwiseUtils;

void APU2A03::Read(uint16_t address, uint8_t& data)
{
	if (address == 0x4015) {
		if (m_frameIRQSent) {
			data = 0x4F;
			m_frameIRQSent = false;
		}
		else {
			data = 0x0F;
		}
	}
	else {
		data = 0;
	}
}

void APU2A03::Write(uint16_t address, uint8_t data)
{
}

uint8_t APU2A03::Probe(uint16_t address)
{
	return 0;
}

void APU2A03::FrameCounterRegisterWrite(uint8_t data) {
	m_frameCounterRegister.value = ClearLowerBits8(data, 6);

	// Frame counter is reset and possibly generate quarter frame and half frame clocks
	// TODO: Do this after 3 cycles?
	m_frameCounter = 0;
	if (m_frameCounterRegister.flags.mode) {
		this->DoFrameCounterQuarter();
		this->DoFrameCounterHalf();
	}
}