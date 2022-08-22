#include "SoundGenerator.h"



static SoundGenerator* s_instance;

SoundGenerator::SoundGenerator(wxEmulationThread* emulationThread) 
{
	s_instance = this;
	m_emulationThread = emulationThread;
	m_deviceID = 0;
	m_soundStopFlag = true;

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		throw SDLException("Error opening Audio device.");
	}
	SDL_memset(&m_audioSpecDesired, 0, sizeof(m_audioSpecDesired));

	m_audioSpecDesired.freq = SOUND_SAMPLE_RATE;
	m_audioSpecDesired.format = AUDIO_F32;
	m_audioSpecDesired.channels = 1;
	m_audioSpecDesired.samples = SOUND_SAMPLE_UPPER_LIMIT;
	m_audioSpecDesired.callback = AudioCallback;
	m_audioSpecDesired.userdata = nullptr;

	m_deviceID = SDL_OpenAudioDevice(SOUND_DEFAULT_DEVICE, 0, &m_audioSpecDesired, &m_audioSpecObtained, 0);
	if (!m_deviceID) {
		throw SDLException("Error opening Audio device.");
	}
}

SoundGenerator::~SoundGenerator()
{
	s_instance = nullptr;
	m_emulationThread = nullptr;
	if (m_deviceID) {
		SDL_CloseAudioDevice(m_deviceID);
	}
}

void SoundGenerator::EnableSound()
{
	m_soundStopFlag = false;
	SDL_PauseAudioDevice(m_deviceID, 0);
}

void SoundGenerator::DisableSound()
{
	m_soundStopFlag = true;
	SDL_PauseAudioDevice(m_deviceID, 1);
}

void SoundGenerator::AudioCallback(void* userdata, uint8_t* stream, int len)
{
	float* resultStream = (float*)stream;
	for (int i = 0; i < (len / sizeof(float)); i++) {
		if (s_instance->m_soundStopFlag) {
			return;
		}
		float sample = SOUND_AMPLITUDE_MODIFIER * s_instance->m_emulationThread->GetAudioSample();
		resultStream[i] = sample;
	}
}
