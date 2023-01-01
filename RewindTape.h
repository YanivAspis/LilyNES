#pragma once

#include <list>
#include "nes/NES.h"

class RewindTape {
public:
	RewindTape(size_t maxSize);
	void RewindAddState(const NESState& state, const NESPicture& picture);
	void RewindStart(const NESState& currState, const NESPicture& currPicture);
	void RewindCancel();
	NESState RewindLoad();
	NESPicture RewindBack();
	NESPicture RewindForward();

private:
	size_t m_rewindTapeMaxSize;
	std::list<NESState> m_stateTape;
	std::list<NESPicture> m_pictureTape;
	std::list<NESState>::iterator m_currState;
	std::list<NESPicture>::iterator m_currPicture;
};
