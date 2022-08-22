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
constexpr float HIGH_PASS_FILTER_1_CUTOFF = 90;
constexpr float HIGH_PASS_FILTER_2_CUTOFF = 440;
constexpr float LOW_PASS_FILTER_CUTOFF = 14000;

class SDLException : public std::exception {
public:
	SDLException(std::string message) : m_message(message) {
		m_message = message + " Details: " + std::string(SDL_GetError());
	}
	const char* what() const noexcept override {
		return m_message.c_str();
	}

private:
	std::string m_message;
};

class HighPassFilter {
public:
	HighPassFilter(float cutoffFrequency, float sampleRate);
	void Restart();
	float Filter(float sample);

private:
	float m_alpha;
	bool m_firstSampleReceived;
	float m_lastOriginalSample;
	float m_lastResampled;
};

class LowPassFilter {
public:
	LowPassFilter(float cutoffFrequency, float sampleRate);
	void Restart();
	float Filter(float sample);

private:
	float m_alpha;
	float m_lastSample;
};

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

	HighPassFilter m_highPassFilter1;
	HighPassFilter m_highPassFilter2;
	LowPassFilter m_lowPassFilter;
};