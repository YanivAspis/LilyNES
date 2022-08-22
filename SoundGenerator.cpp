#include "SoundGenerator.h"

HighPassFilter::HighPassFilter(float cutoffFrequency, float sampleRate) {
	float RC = 1.0 / (2 * M_PI * cutoffFrequency);
	float dt = 1.0 / sampleRate;
	m_alpha = RC / (RC + dt);
	m_firstSampleReceived = false;
	m_lastOriginalSample = 0;
	m_lastResampled = 0;
}

void HighPassFilter::Restart() {
	m_firstSampleReceived = false;
	m_lastOriginalSample = 0;
	m_lastResampled = 0;
}

float HighPassFilter::Filter(float sample) {
	if (m_firstSampleReceived) {
		m_lastResampled = m_alpha * (m_lastResampled + sample - m_lastOriginalSample);
		m_lastOriginalSample = sample;
	}
	else {
		m_lastResampled = sample;
		m_lastOriginalSample = sample;
		m_firstSampleReceived = true;
	}
	return m_lastResampled;
}

LowPassFilter::LowPassFilter(float cutoffFrequency, float sampleRate) {
	float RC = 1.0 / (2 * M_PI * cutoffFrequency);
	float dt = 1.0 / sampleRate;
	m_alpha = dt / (RC + dt);
	m_lastSample = 0;
}

void LowPassFilter::Restart() {
	m_lastSample = 0;
}

float LowPassFilter::Filter(float sample) {
	m_lastSample += m_alpha * (sample - m_lastSample);
	return m_lastSample;
}

static SoundGenerator* s_instance;

SoundGenerator::SoundGenerator(wxEmulationThread* emulationThread) : m_highPassFilter1(HIGH_PASS_FILTER_1_CUTOFF, SOUND_SAMPLE_RATE), m_highPassFilter2(HIGH_PASS_FILTER_2_CUTOFF, SOUND_SAMPLE_RATE), m_lowPassFilter(LOW_PASS_FILTER_CUTOFF, SOUND_SAMPLE_RATE)
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
	m_highPassFilter1.Restart();
	m_highPassFilter2.Restart();
	m_lowPassFilter.Restart();
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
		sample = s_instance->m_highPassFilter1.Filter(sample);
		sample = s_instance->m_highPassFilter2.Filter(sample);
		sample = s_instance->m_lowPassFilter.Filter(sample);
		resultStream[i] = sample;
	}
}
