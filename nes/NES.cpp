#include "NES.h"


NES::NES() {
	m_cpuBus.ConnectDevice(&m_RAM);
	m_cpu.ConnectToBus(&m_cpuBus);
}

void NES::SoftReset()
{
	m_cpuBus.SoftReset();
	m_cpu.SoftReset();
}

void NES::HardReset()
{
	m_cpuBus.HardReset();
	m_cpu.HardReset();
}

NESState NES::GetState() const
{
	NESState state;
	state.cpuState = m_cpu.GetState();
	state.ramState = m_RAM.GetState();
	return state;
}

void NES::LoadState(NESState& state)
{
	m_cpu.LoadState(state.cpuState);
	m_RAM.LoadState(state.ramState);
}

void NES::Clock()
{
	m_cpu.Clock();
}
