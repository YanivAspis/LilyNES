#pragma once

#include "OAM.h"

constexpr unsigned int OAM_ENTRY_SIZE = 4;
constexpr unsigned int SECONDARY_OAM_SIZE = 8;
constexpr unsigned int OAM_NO_SPRITE_ID = 64;
constexpr unsigned int SECONDARY_OAM_INTERNAL_INITIALIZATION_DOT = 0;
constexpr unsigned int SECONDARY_OAM_INITIALIZATION_START_DOT = 1;
constexpr unsigned int SECONDARY_OAM_INITIALIZATION_END_DOT = 64;
constexpr unsigned int SECONDARY_OAM_EVALUATION_START_DOT = 65;
constexpr unsigned int SECONDARY_OAM_EVALUATION_END_DOT = 256;
constexpr unsigned int SECONDARY_OAM_SPRITE_FETCH_START_DOT = 257;
constexpr unsigned int SECONDARY_OAM_SPRITE_FETCH_END_DOT = 320;
constexpr unsigned int SECONDARY_OAM_BACKGROUND_RENDER_START_DOT = 321;
constexpr unsigned int SECONDARY_OAM_BACKGROUND_RENDER_END_DOT = 340;

constexpr unsigned int SECONDARY_OAM_ADDITIONAL_OVERFLOW_READS = 3;


struct SecondaryOAMEntry {
	OAMEntry entry;
	unsigned int spriteID; // 0 - 64. 64 == No sprite present
};

enum SpriteEvaluationStep {
	SPRITE_EVALUATION_SPRITE_SEARCH,
	SPRITE_EVALUATION_SPRITE_COPY,
	SPRITE_EVALUATION_SPRITE_OVERFLOW_SEARCH,
	SPRITE_EVALUATION_SPRITE_OVERFLOW_FOUND,
	SPRITE_EVALUATION_WAIT_FOR_HBLANK
};

struct SecondaryOAMState {
	SecondaryOAMState();
	std::array<SecondaryOAMEntry, SECONDARY_OAM_SIZE> entries;
	uint8_t internalBuffer;
	//unsigned int byteIndex;
	//unsigned int spriteIndex;
	//unsigned int entriesAdded;
	//SpriteEvaluationStep evaluationStep;
	//unsigned int overflowFoundReadsLeft;
	bool spriteOverflowDetected;
};

class SecondaryOAM {
public:
	SecondaryOAM(OAM* primaryOAM);

	void SoftReset();
	void HardReset();

	void SecondaryOAMInitialization();
	void SpriteEvaluation(unsigned int scanline, bool mode8by16);

	//void Clock(unsigned int scanline, unsigned int dot, bool mode8by16);

	SecondaryOAMEntry& GetEntry(unsigned int entryIndex);
	uint8_t GetInternalBuffer() const;
	bool SpriteOverflowDetected() const;

	SecondaryOAMState GetState() const;
	void LoadState(SecondaryOAMState& state);

private:


	//void Initialize();

	uint8_t ReadPrimaryOAM(unsigned int entryIndex, unsigned int byteIndex);
	/*
	void WriteToEntry(unsigned int entryIndex, unsigned int byteIndex, uint8_t data);

	void SecondaryOAMInitializationRead();
	void SecondaryOAMInitializationWrite(unsigned int dot);

	void SpriteEvaluationRead();
	void SpriteEvaluationWrite(unsigned int scanline, bool mode8by16);

	void SpriteSearchRead();
	void SpriteSearchWrite(unsigned int scanline, bool mode8by16);

	void SpriteCopyRead();
	void SpriteCopyWrite();

	void SpriteOverflowSearchRead();
	void SpriteOverflowSearchWrite(unsigned int scanline, bool mode8by16);

	void SpriteOverflowFoundRead();
	void SpriteOverflowFoundWrite();

	void WaitForHBlankRead();
	void WaitForHBlankWrite();

	void SpriteFetchRead(unsigned int dot);
	void BackgroundRenderRead();
	*/

	bool IsSpriteOnScanline(unsigned int scanline, uint8_t y, bool mode8by16) const;

	std::array<SecondaryOAMEntry, SECONDARY_OAM_SIZE> m_entries;
	OAM* m_primaryOAM;

	uint8_t m_internalBuffer; 
	//unsigned int m_byteIndex; // Index of byte in OAM entry (0 - 3)
	//unsigned int m_spriteIndex; // Index of sprite in primary OAM (0 - 63, 64 == overflow back to 0)
	//unsigned int m_entriesAdded; // 0-8.
	//SpriteEvaluationStep m_evaluationStep;
	//unsigned int m_overflowFoundReadsLeft;
	bool m_spriteOverflowDetected;
};
