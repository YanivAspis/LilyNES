#include "RewindTape.h"

RewindTape::RewindTape(size_t maxSize) {
	m_rewindTapeMaxSize = maxSize;
	m_currState = m_stateTape.end();
	m_currPicture = m_pictureTape.end();
}

void RewindTape::RewindAddState(const NESState& state, const NESPicture& picture) {
	if (m_stateTape.size() == m_rewindTapeMaxSize) {
		m_stateTape.pop_front();
		m_pictureTape.pop_front();
	}
	m_stateTape.push_back(state);
	m_pictureTape.push_back(picture);
}

void RewindTape::RewindStart(const NESState& currState, const NESPicture& currPicture) {
	m_stateTape.push_back(currState);
	m_pictureTape.push_back(currPicture);
	m_currState = m_stateTape.end();
	m_currState--;
	m_currPicture = m_pictureTape.end();
	m_currPicture--;
}

void RewindTape::RewindCancel() {
	m_stateTape.pop_back();
	m_pictureTape.pop_back();
	m_currState = m_stateTape.end();
	m_currPicture = m_pictureTape.end();
}

NESState RewindTape::RewindLoad() {
	NESState state = *m_currState;
	m_currState++;
	m_currPicture++;
	m_stateTape.erase(m_currState, m_stateTape.end());
	m_pictureTape.erase(m_currPicture, m_pictureTape.end());
	m_currState = m_stateTape.end();
	m_currPicture = m_pictureTape.end();
	return state;

}

NESPicture RewindTape::RewindBack() {
	if (m_currState != m_stateTape.begin()) {
		m_currState--;
		m_currPicture--;
	}
	return *m_currPicture;
}

NESPicture RewindTape::RewindForward() {
	m_currState++;
	if (m_currState == m_stateTape.end()) {
		m_currState--;
	}
	else {
		m_currPicture++;
	}
	return *m_currPicture;
}