#include "SecondaryOAM.h"

SecondaryOAMEntryState::SecondaryOAMEntryState() {
	spriteID = OAM_NO_SPRITE_ID;
}

SecondaryOAMEntryState::SecondaryOAMEntryState(const SecondaryOAMEntry& actualEntry) {
	entry = actualEntry.entry;
	spriteID = actualEntry.spriteID;
}

SecondaryOAMEntryState& SecondaryOAMEntryState::operator=(const SecondaryOAMEntry& actualEntry) {
	entry = actualEntry.entry;
	spriteID = actualEntry.spriteID;
	return *this;
}

SecondaryOAMEntry::SecondaryOAMEntry() {
	spriteID = OAM_NO_SPRITE_ID;
}

SecondaryOAMEntry::SecondaryOAMEntry(const SecondaryOAMEntryState& entryState) {
	entry = entryState.entry;
	spriteID = entryState.spriteID;
}

SecondaryOAMEntry& SecondaryOAMEntry::operator=(const SecondaryOAMEntryState& entryState) {
	entry = entryState.entry;
	spriteID = entryState.spriteID;
	return *this;
}

SecondaryOAMState::SecondaryOAMState() {
	internalBuffer = OAM_INITIAL_VALUE;
	//byteIndex = 0;
	//spriteIndex = 0;
	//entriesAdded = 0;
	//evaluationStep = SPRITE_EVALUATION_SPRITE_SEARCH;
	//overflowFoundReadsLeft = SECONDARY_OAM_ADDITIONAL_OVERFLOW_READS;
	spriteOverflowDetected = false;
}

SecondaryOAM::SecondaryOAM(OAM* primaryOAM)
{
	m_primaryOAM = primaryOAM;
	m_internalBuffer = OAM_INITIAL_VALUE;
	//m_byteIndex = 0;
	//m_spriteIndex = 0;
	//m_entriesAdded = 0;
	//m_evaluationStep = SPRITE_EVALUATION_SPRITE_SEARCH;
	//m_overflowFoundReadsLeft = SECONDARY_OAM_ADDITIONAL_OVERFLOW_READS;
	m_spriteOverflowDetected = false;
}

void SecondaryOAM::SoftReset() {
	m_internalBuffer = OAM_INITIAL_VALUE;
	//m_byteIndex = 0;
	//m_spriteIndex = 0;
	//m_entriesAdded = 0;
	//m_evaluationStep = SPRITE_EVALUATION_SPRITE_SEARCH;
	//m_overflowFoundReadsLeft = SECONDARY_OAM_ADDITIONAL_OVERFLOW_READS;
	m_spriteOverflowDetected = false;

	for (SecondaryOAMEntry& entry : m_entries) {
		entry.spriteID = OAM_NO_SPRITE_ID;
	}
}

void SecondaryOAM::HardReset() {
	m_internalBuffer = OAM_INITIAL_VALUE;
	//m_byteIndex = 0;
	//m_spriteIndex = 0;
	//m_entriesAdded = 0;
	//m_evaluationStep = SPRITE_EVALUATION_SPRITE_SEARCH;
	//m_overflowFoundReadsLeft = SECONDARY_OAM_ADDITIONAL_OVERFLOW_READS;
	m_spriteOverflowDetected = false;

	for (SecondaryOAMEntry& entry : m_entries) {
		entry.spriteID = OAM_NO_SPRITE_ID;
	}
}

void SecondaryOAM::SecondaryOAMInitialization() {
	for (SecondaryOAMEntry& entry : m_entries) {
		entry.entry.y = OAM_INITIAL_VALUE;
		entry.entry.tileID = OAM_INITIAL_VALUE;
		entry.entry.attribute.value = OAM_INITIAL_VALUE & OAM_ATTRIBUTE_UNUSED_MASK;
		entry.entry.x = OAM_INITIAL_VALUE;
		entry.spriteID = OAM_NO_SPRITE_ID;
	}

	m_internalBuffer = OAM_INITIAL_VALUE;
	m_spriteOverflowDetected = false;
}

void SecondaryOAM::SpriteEvaluation(unsigned int scanline, bool mode8by16) {
	// Search for up to 8 sprites in scanline
	unsigned int spriteIndex = 0;
	unsigned int entriesAdded = 0;

	for (; spriteIndex < OAM_NUM_SPRITES; ) {
		OAMEntry entry = m_primaryOAM->GetEntry(spriteIndex);
		if (this->IsSpriteOnScanline(scanline, entry.y, mode8by16)) {
			m_entries[entriesAdded].entry.y = entry.y;
			m_entries[entriesAdded].entry.tileID = entry.tileID;
			m_entries[entriesAdded].entry.attribute.value = entry.attribute.value;
			m_entries[entriesAdded].entry.x = entry.x;
			m_entries[entriesAdded].spriteID = spriteIndex;
			entriesAdded++;
			if (entriesAdded == SECONDARY_OAM_SIZE) {
				spriteIndex++;
				break;
			}
		}
		spriteIndex++;
	}

	// Search for sprite overflow if entries are left
	unsigned int byteIndex = 0;
	while (spriteIndex < OAM_NUM_SPRITES) {
		uint8_t effectiveY = this->ReadPrimaryOAM(spriteIndex, byteIndex);
		if (this->IsSpriteOnScanline(scanline, effectiveY, mode8by16)) {
			m_spriteOverflowDetected = true;
			break;
		}
		spriteIndex++;
		byteIndex++; // Sprite overflow bug
		if (byteIndex == OAM_ENTRY_SIZE) {
			byteIndex = 0;
		}
	}

	// TODO: deal with internal buffer?
}

/*
void SecondaryOAM::Clock(unsigned int scanline, unsigned int dot, bool mode8by16)
{
	if (dot == SECONDARY_OAM_INTERNAL_INITIALIZATION_DOT) {
		this->Initialize();
	}
	else if (dot >= SECONDARY_OAM_INITIALIZATION_START_DOT && dot <= SECONDARY_OAM_INITIALIZATION_END_DOT) {
		if (dot % 2 != 0) {
			this->SecondaryOAMInitializationRead();
		}
		else {
			this->SecondaryOAMInitializationWrite(dot);
		}
	}
	else if (dot >= SECONDARY_OAM_EVALUATION_START_DOT && dot <= SECONDARY_OAM_EVALUATION_END_DOT) {
		if (dot % 2 != 0) {
			this->SpriteEvaluationRead();
		}
		else {
			this->SpriteEvaluationWrite(scanline, mode8by16);
		}
	}
	else if (dot >= SECONDARY_OAM_SPRITE_FETCH_START_DOT && dot <= SECONDARY_OAM_SPRITE_FETCH_END_DOT) {
		this->SpriteFetchRead(dot);
	}
	else if (dot >= SECONDARY_OAM_BACKGROUND_RENDER_START_DOT && dot <= SECONDARY_OAM_BACKGROUND_RENDER_END_DOT) {
		this->BackgroundRenderRead();
	}
}
*/

SecondaryOAMEntry& SecondaryOAM::GetEntry(unsigned int entryIndex)
{
	return m_entries[entryIndex];
}

uint8_t SecondaryOAM::GetInternalBuffer() const {
	return m_internalBuffer;
}


bool SecondaryOAM::SpriteOverflowDetected() const
{
	return m_spriteOverflowDetected;
}

SecondaryOAMState SecondaryOAM::GetState() const {
	SecondaryOAMState state;
	for (size_t i = 0; i < SECONDARY_OAM_SIZE; i++) {
		state.entries[i] = m_entries[i];
	}
	state.internalBuffer = m_internalBuffer;
	//state.byteIndex = m_byteIndex;
	//state.spriteIndex = m_spriteIndex;
	//state.entriesAdded = m_entriesAdded;
	//state.evaluationStep = m_evaluationStep;
	//state.overflowFoundReadsLeft = m_overflowFoundReadsLeft;
	state.spriteOverflowDetected = m_spriteOverflowDetected;
	return state;
}

void SecondaryOAM::LoadState(SecondaryOAMState& state) {
	for (size_t i = 0; i < SECONDARY_OAM_SIZE; i++) {
		m_entries[i] = state.entries[i];
	}
	m_internalBuffer = state.internalBuffer;
	//m_byteIndex = m_byteIndex;
	//m_spriteIndex = m_spriteIndex;
	//m_entriesAdded = state.entriesAdded;
	//m_evaluationStep = state.evaluationStep;
	//m_overflowFoundReadsLeft = state.overflowFoundReadsLeft;
	m_spriteOverflowDetected = state.spriteOverflowDetected;
}

/*
void SecondaryOAM::Initialize()
{
	m_internalBuffer = m_entries[0].entry.y;
	m_byteIndex = 0;
	m_spriteIndex = 0;
	m_entriesAdded = 0;
	m_evaluationStep = SPRITE_EVALUATION_SPRITE_SEARCH;
	m_spriteOverflowDetected = false;
	m_overflowFoundReadsLeft = SECONDARY_OAM_ADDITIONAL_OVERFLOW_READS;
}
*/

uint8_t SecondaryOAM::ReadPrimaryOAM(unsigned int entryIndex, unsigned int byteIndex)
{
	OAMEntry entry = m_primaryOAM->GetEntry(entryIndex);
	switch (byteIndex) {
	case 0:
		return entry.y;
	case 1:
		return entry.tileID;
	case 2:
		return entry.attribute.value & OAM_ATTRIBUTE_UNUSED_MASK;
	case 3:
		return entry.x;
	}
	return 0;
}

/*
void SecondaryOAM::WriteToEntry(unsigned int entryIndex, unsigned int byteIndex, uint8_t data)
{
	switch (byteIndex) {
	case 0:
		m_entries[entryIndex].entry.y = data;
		break;
	case 1:
		m_entries[entryIndex].entry.tileID = data;
		break;
	case 2:
		m_entries[entryIndex].entry.attribute.value = data & OAM_ATTRIBUTE_UNUSED_MASK;
		break;
	case 3:
		m_entries[entryIndex].entry.x = data;
		break;
	}
}

void SecondaryOAM::SecondaryOAMInitializationRead()
{
	m_internalBuffer = OAM_INITIAL_VALUE;
}

void SecondaryOAM::SecondaryOAMInitializationWrite(unsigned int dot)
{
	unsigned int initializationCycle = (dot - SECONDARY_OAM_INITIALIZATION_START_DOT) / 2;
	this->WriteToEntry(initializationCycle / OAM_ENTRY_SIZE, initializationCycle % OAM_ENTRY_SIZE, m_internalBuffer);
	m_entries[initializationCycle / OAM_ENTRY_SIZE].spriteID = OAM_NO_SPRITE_ID;
}

void SecondaryOAM::SpriteEvaluationRead() {
	switch (m_evaluationStep) {
	case SPRITE_EVALUATION_SPRITE_SEARCH:
		this->SpriteSearchRead();
		break;
	case SPRITE_EVALUATION_SPRITE_COPY:
		this->SpriteCopyRead();
		break;
	case SPRITE_EVALUATION_SPRITE_OVERFLOW_SEARCH:
		this->SpriteOverflowSearchRead();
		break;
	case SPRITE_EVALUATION_SPRITE_OVERFLOW_FOUND:
		this->SpriteOverflowFoundRead();
		break;
	case SPRITE_EVALUATION_WAIT_FOR_HBLANK:
		this->WaitForHBlankRead();
		break;
	}
}

void SecondaryOAM::SpriteEvaluationWrite(unsigned int scanline, bool mode8by16) {
	switch (m_evaluationStep) {
	case SPRITE_EVALUATION_SPRITE_SEARCH:
		this->SpriteSearchWrite(scanline, mode8by16);
		break;
	case SPRITE_EVALUATION_SPRITE_COPY:
		this->SpriteCopyWrite();
		break;
	case SPRITE_EVALUATION_SPRITE_OVERFLOW_SEARCH:
		this->SpriteOverflowSearchWrite(scanline, mode8by16);
		break;
	case SPRITE_EVALUATION_SPRITE_OVERFLOW_FOUND:
		this->SpriteOverflowFoundWrite();
		break;
	case SPRITE_EVALUATION_WAIT_FOR_HBLANK:
		this->WaitForHBlankWrite();
		break;
	}
}

void SecondaryOAM::SpriteSearchRead()
{
	m_internalBuffer = this->ReadPrimaryOAM(m_spriteIndex, 0);
}

void SecondaryOAM::SpriteSearchWrite(unsigned int scanline, bool mode8by16)
{
	this->WriteToEntry(m_entriesAdded, 0, m_internalBuffer);
	if (this->IsSpriteOnScanline(scanline, m_internalBuffer, mode8by16)) {
		m_evaluationStep = SPRITE_EVALUATION_SPRITE_COPY;
		m_byteIndex++;
	}
	else {
		m_spriteIndex++;
		if (m_spriteIndex == OAM_NUM_SPRITES) {
			// Finished examining all sprites (found less than 8 in this line), so wait for HBLANK
			m_spriteIndex = 0;
			m_evaluationStep = SPRITE_EVALUATION_WAIT_FOR_HBLANK;
		}
	}
	
}

void SecondaryOAM::SpriteCopyRead()
{
	m_internalBuffer = this->ReadPrimaryOAM(m_spriteIndex, m_byteIndex);
}

void SecondaryOAM::SpriteCopyWrite()
{
	this->WriteToEntry(m_entriesAdded, m_byteIndex, m_internalBuffer);
	m_entries[m_entriesAdded].spriteID = m_spriteIndex;
	m_byteIndex++;
	if (m_byteIndex == OAM_ENTRY_SIZE) {
		// Finished copying sprite to secondary OAM
		m_byteIndex = 0;
		m_spriteIndex++;
		m_entriesAdded++;
		if (m_spriteIndex == OAM_NUM_SPRITES) {
			// Finished examining all sprites, so wait for HBLANK
			m_spriteIndex = 0;
			m_evaluationStep = SPRITE_EVALUATION_WAIT_FOR_HBLANK;
		}
		else if (m_entriesAdded == SECONDARY_OAM_SIZE) {
			// Found 8 sprites on the scanline but there are still sprites to examine, begin searching for sprite overflow
			m_evaluationStep = SPRITE_EVALUATION_SPRITE_OVERFLOW_SEARCH;
		}
		else {
			// There are still sprites to examine and we haven't found 8 yet, so continue searching
			m_evaluationStep = SPRITE_EVALUATION_SPRITE_SEARCH;
		}
	}
}

void SecondaryOAM::SpriteOverflowSearchRead()
{
	m_internalBuffer = this->ReadPrimaryOAM(m_spriteIndex, m_byteIndex);
}

void SecondaryOAM::SpriteOverflowSearchWrite(unsigned int scanline, bool mode8by16)
{
	if (this->IsSpriteOnScanline(scanline, m_internalBuffer, mode8by16)) {
		// An additional sprite has been found - set sprite overflow flag
		m_spriteOverflowDetected = true;
		m_evaluationStep = SPRITE_EVALUATION_SPRITE_OVERFLOW_FOUND;
		m_byteIndex++;
		if (m_byteIndex == OAM_ENTRY_SIZE) {
			m_byteIndex = 0;
			m_spriteIndex++;
			if (m_spriteIndex == OAM_NUM_SPRITES) {
				m_spriteIndex = 0;
			}
		}
	}
	else {
		// Sprite doesn't cause overflow, increment sprite index to check next
		// NES Bug - Increment byte index as well (so the wrong byte is interpreted as y) - sprite overflow flag is buggy
		m_byteIndex++;
		if (m_byteIndex == OAM_ENTRY_SIZE) {
			m_byteIndex = 0;
		}
		m_spriteIndex++;
		if (m_spriteIndex == OAM_NUM_SPRITES) {
			// We finished checking all sprites and found no overflow, so wait for HBlank
			m_spriteIndex = 0;
			m_evaluationStep = SPRITE_EVALUATION_WAIT_FOR_HBLANK;
		}
	}
}

void SecondaryOAM::SpriteOverflowFoundRead()
{
	m_internalBuffer = this->ReadPrimaryOAM(m_spriteIndex, m_byteIndex);
}

void SecondaryOAM::SpriteOverflowFoundWrite()
{
	m_byteIndex++;
	if (m_byteIndex == OAM_ENTRY_SIZE) {
		m_byteIndex = 0;
		m_spriteIndex++;
		if (m_spriteIndex == OAM_NUM_SPRITES) {
			m_spriteIndex = 0;
		}
	}
	m_overflowFoundReadsLeft--;
	if (m_overflowFoundReadsLeft == 0) {
		// Finished additional reads, all that's left is to wait for HBLANK
		m_evaluationStep = SPRITE_EVALUATION_WAIT_FOR_HBLANK;
	}
}

void SecondaryOAM::WaitForHBlankRead()
{
	m_internalBuffer = this->ReadPrimaryOAM(m_spriteIndex, 0);
}

void SecondaryOAM::WaitForHBlankWrite()
{
	m_spriteIndex++;
	if (m_spriteIndex == OAM_NUM_SPRITES) {
		m_spriteIndex = 0;
	}
}

void SecondaryOAM::SpriteFetchRead(unsigned int dot)
{
	unsigned int entryIndex = (dot - SECONDARY_OAM_SPRITE_FETCH_START_DOT) / SECONDARY_OAM_SIZE;
	unsigned int entryCycle = (dot - SECONDARY_OAM_SPRITE_FETCH_START_DOT) % SECONDARY_OAM_SIZE;
	switch (entryCycle) {
	case 0:
		m_internalBuffer = m_entries[entryIndex].entry.y;
		break;
	case 1:
		m_internalBuffer = m_entries[entryIndex].entry.tileID;
		break;
	case 2:
		m_internalBuffer = m_entries[entryIndex].entry.attribute.value & OAM_ATTRIBUTE_UNUSED_MASK;
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		m_internalBuffer = m_entries[entryIndex].entry.x;
	}
}

void SecondaryOAM::BackgroundRenderRead()
{
	m_internalBuffer = m_entries[0].entry.y;
}*/

bool SecondaryOAM::IsSpriteOnScanline(unsigned int scanline, uint8_t y, bool mode8by16) const {
	unsigned int yOffset = mode8by16 ? 16 : 8;
	return (scanline >= y) && (scanline < (y + yOffset));
}