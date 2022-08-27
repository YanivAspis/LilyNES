#include "NametableDevice.h"

NametableState::NametableState() {
	nametable0.fill(0);
	nametable1.fill(0);
}

NametableDevice::NametableDevice() : BusDevice(std::list<AddressRange>({ AddressRange(NAMETABLE_BEGIN_ADDRESS, NAMETABLE_END_ADDRESS) })),
	m_cartridge(nullptr) {

	m_nametable0.fill(0);
	m_nametable1.fill(0);

	m_mirroringFunctions[MIRRORING_HORIZONTAL] = &NametableDevice::GetNametablePointerHorizontalMirroring;
	m_mirroringFunctions[MIRRORING_VERTICAL] = &NametableDevice::GetNametablePointerVerticalMirroring;
	m_mirroringFunctions[MIRRORING_SINGLE_SCREEN] = &NametableDevice::GetNametablePointerSingleScreenMirroring;
	
}

NametableDevice::~NametableDevice() {
	this->DisconnectCartridge();
}

// Nametable RAM unchanged on reset
void NametableDevice::SoftReset() {}

// Nametable RAM unspecified on power up, I'll set it all to 0's
void NametableDevice::HardReset() {
	m_nametable0.fill(0);
	m_nametable1.fill(0);
}

void NametableDevice::Read(uint16_t address, uint8_t& data) {
	uint8_t* dataAddress = this->GetDataPointer(address);
	data = *dataAddress;
}

void NametableDevice::Write(uint16_t address, uint8_t data) {
	uint8_t* dataAddress = this->GetDataPointer(address);
	*dataAddress = data;
}

uint8_t NametableDevice::Probe(uint16_t address) {
	uint8_t data = 0;
	this->Read(address, data);
	return data;
}

NametableState NametableDevice::GetState() const {
	NametableState state;
	state.nametable0 = m_nametable0;
	state.nametable1 = m_nametable1;
	return state;
}

void NametableDevice::LoadState(NametableState& state) {
	m_nametable0 = state.nametable0;
	m_nametable1 = state.nametable1;
}

void NametableDevice::ConnectCartridge(Cartridge* cartridge) {
	assert(m_cartridge == nullptr);
	m_cartridge = cartridge;
}

void NametableDevice::DisconnectCartridge() {
	m_cartridge = nullptr;
}

// Translate loopy reg contents into nametable content address
uint16_t NametableDevice::GetNametableByteAddress(LoopyRegister VRAMaddress) {
	VRAMaddress.scrollFlags.fineY = 2;
	return VRAMaddress.address;
}

// Translate loopy reg contents into attribute (palette) byte address
uint16_t NametableDevice::GetAttributeByteAddress(LoopyRegister VRAMaddress) {
	return ShiftRight16(VRAMaddress.scrollFlags.coarseX, 2) |
		ShiftLeft16(ShiftRight16(VRAMaddress.scrollFlags.coarseY, 2), 3) |
		NAMETABLE_ATTRIBUTE_OFFSET |
		ShiftLeft16(VRAMaddress.scrollFlags.nametableX, 10) |
		ShiftLeft16(VRAMaddress.scrollFlags.nametableY, 11) |
		NAMETABLE_BEGIN_ADDRESS;
}

// Extract palette index from palette byte
uint8_t NametableDevice::GetPaletteFromAttributeByte(LoopyRegister VRAMaddress, uint8_t attributeByte) {
	unsigned int leftOrRight = (VRAMaddress.scrollFlags.coarseX % NAMETABLE_ATTRIBUTE_NUM_TILES_PER_BYTE) >> 1;
	unsigned int upOrDown = (VRAMaddress.scrollFlags.coarseY % NAMETABLE_ATTRIBUTE_NUM_TILES_PER_BYTE) >> 1;
	uint8_t lowBit;
	uint8_t highBit;
	if (leftOrRight && !upOrDown) {
		// top right
		lowBit = TestBit8(attributeByte, 2);
		highBit = TestBit8(attributeByte, 3);
	}
	else if (!leftOrRight && !upOrDown) {
		// top left
		lowBit = TestBit8(attributeByte, 0);
		highBit = TestBit8(attributeByte, 1);
	}
	else if (leftOrRight && upOrDown) {
		// bottom right
		lowBit = TestBit8(attributeByte, 6);
		highBit = TestBit8(attributeByte, 7);
	}
	else {
		// bottom left
		lowBit = TestBit8(attributeByte, 4);
		highBit = TestBit8(attributeByte, 5);
	}
	return ShiftLeft8(highBit, 1) | lowBit;
}

uint8_t* NametableDevice::GetDataPointer(uint16_t address) {
	// Map address to 0x0000 - 0x0FFF - this takes care of mirroring from 0x3000-03EFF
	address = ClearUpperBits16(address, 12);
	std::array<uint8_t, NAMETABLE_SIZE>* nametable = m_mirroringFunctions[m_cartridge->GetCurrentMirroringMode()](*this, address);
	unsigned int byteIndex = ClearUpperBits16(address, 10);
	return &((*nametable)[byteIndex]);
}

std::array<uint8_t, NAMETABLE_SIZE>* NametableDevice::GetNametablePointerHorizontalMirroring(uint16_t address) {
	if (address < 2 * NAMETABLE_SIZE) {
		return &m_nametable0;
	}
	else {
		return &m_nametable1;
	}
}

std::array<uint8_t, NAMETABLE_SIZE>* NametableDevice::GetNametablePointerVerticalMirroring(uint16_t address) {
	if (address < NAMETABLE_SIZE || (address >= 2 * NAMETABLE_SIZE && address < 3 * NAMETABLE_SIZE)) {
		return &m_nametable0;
	}
	else {
		return &m_nametable1;
	}
}

std::array<uint8_t, NAMETABLE_SIZE>* NametableDevice::GetNametablePointerSingleScreenMirroring(uint16_t address) {
	return &m_nametable0;
}
