#pragma once

#include <stdio.h>
#include <cstdint>
#include <SDL.h>
#include <queue>
#include <functional>
#include <exception>
#include <string>

#include "GUI/wxEmulationThread.h"

constexpr int SOUND_SAMPLE_RATE = 44100;
constexpr size_t SOUND_SAMPLE_UPPER_LIMIT = 512;
constexpr char* SOUND_DEFAULT_DEVICE = nullptr;

constexpr double SOUND_AMPLITUDE_MODIFIER = 1.0;


class wxEmulationThread;

class SoundGenerator {
public:
	SoundGenerator(wxEmulationThread* emulationThread);
	~SoundGenerator();
	void EnableSound();
	void DisableSound();

	static void AudioCallback(void* userdata, uint8_t* stream, int len);

private:
	wxEmulationThread* m_emulationThread;
	bool m_soundStopFlag;

	SDL_AudioSpec m_audioSpecDesired;
	SDL_AudioSpec m_audioSpecObtained;
	SDL_AudioDeviceID m_deviceID;


};