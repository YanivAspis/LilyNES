#include "OAM.h"

OAMEntry::OAMEntry() {
	y = 0;
	tileID = 0;
	attribute.value = 0;
	x = 0;
}

// OAM Entries are unspecified on power/reset. I'll set them all to 0
void OAM::SoftReset() {
	for (OAMEntry& entry : m_entries) {
		entry.y = 0;
		entry.tileID = 0;
		entry.attribute.value = 0;
		entry.x = 0;
	}
}

void OAM::HardReset() {
	for (OAMEntry& entry : m_entries) {
		entry.y = 0;
		entry.tileID = 0;
		entry.attribute.value = 0;
		entry.x = 0;
	}
}

// used to address OAM for individual bytes (0 - 255)
uint8_t OAM::Read(uint8_t address) {
	unsigned int index = address / 4;
	switch (address % 4) {
	case 0:
		return m_entries[index].y;
	case 1:
		return m_entries[index].tileID;
	case 2:
		return m_entries[index].attribute.value & OAM_ATTRIBUTE_UNUSED_MASK;
	case 3:
		return m_entries[index].x;
	}
	return 0;
}

void OAM::Write(uint8_t address, uint8_t data) {
	unsigned int index = address / 4;
	switch (address % 4) {
	case 0:
		m_entries[index].y = data;
		break;
	case 1:
		m_entries[index].tileID = data;
		break;
	case 2:
		m_entries[index].attribute.value = data & OAM_ATTRIBUTE_UNUSED_MASK;
		break;
	case 3:
		m_entries[index].x = data;
		break;
	}
}

OAMState OAM::GetState() const {
	OAMState state;
	state.entries = m_entries;
	return state;
}

void OAM::LoadState(OAMState& state) {
	m_entries = state.entries;
}

// used to address OAM for enrties (0-63)
OAMEntry& OAM::GetEntry(unsigned int index) {
	return m_entries[index];
}