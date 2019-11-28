#include "audio_system.h"

#include <chrono>
#include <thread>

#include "audio_static_buffer.h"
#include "audio_streaming_buffer.h"
#include "audio_source.h"
#include "core/resource_data.h"

String AudioSystem::GetOpenALErrorString(int errID)
{
	switch (errID)
	{
	case AL_NO_ERROR:
		return "OK";
	case AL_INVALID_NAME:
		return "Invalid Name";
	case AL_INVALID_ENUM:
		return "Invalid Enum";
	case AL_INVALID_VALUE:
		return "Invalid Value";
	case AL_INVALID_OPERATION:
		return "Invalid Operation";
	case AL_OUT_OF_MEMORY:
		return "Out of Memory";
	}
	return "Unknown Error: " + std::to_string(errID);
}

void AudioSystem::CheckOpenALError(const char* msg, const char* fname, int line)
{
	ALenum err = alGetError();
	if (err != AL_NO_ERROR)
	{
		ERR("OpenAL error (" + std::to_string(err) + "): " + GetOpenALErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

void AudioSystem::CheckALUTError(const char* msg, const char* fname, int line)
{
	ALenum err = alutGetError();
	if (err != ALUT_ERROR_NO_ERROR)
	{
		ERR("ALUT error (" + std::to_string(err) + "): " + alutGetErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

bool AudioSystem::initialize()
{
	alGetError();

	const char* ALDevice = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	m_Device = alcOpenDevice(ALDevice);

	if (m_Device == nullptr)
	{
		ERR("AudioSystem: Failed to Initialize OpenAL device");
		return false;
	}

	m_Context = alcCreateContext(m_Device, nullptr);
	AL_CHECK(alcMakeContextCurrent(m_Context));

	if (!alutInitWithoutContext(NULL, NULL))
	{
		ERR("AudioSystem: ALUT failed to initialize");
		return false;
	}

	return true;
}

void AudioSystem::update()
{
	while (true)
	{
		for (auto& source : m_ActiveAudioSources)
		{
			source->queueNewBuffers();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(m_UpdateInterval));
	}
}

AudioSystem* AudioSystem::GetSingleton()
{
	static AudioSystem singleton;

	return &singleton;
}

void AudioSystem::shutDown()
{
	alcCloseDevice(m_Device);

	PANIC(m_ActiveAudioSources.size() > 0, "AudioSystem: AL streaming buffers are still alive. Deleting leaked buffers.");

	alutExit();
}

void AudioSystem::registerInstance(AudioSource* audio)
{
	m_ActiveAudioSources.push_back(audio);
}

void AudioSystem::deregisterInstance(AudioSource* audio)
{
	auto findIt = std::find(m_ActiveAudioSources.begin(), m_ActiveAudioSources.end(), audio);
	if (findIt != m_ActiveAudioSources.end())
	{
		m_ActiveAudioSources.erase(findIt);
		return;
	}

	WARN("AudioSystem: Tried to double deregisterInstance a AudioStreamingBuffer. Delete aborted.");
}

void AudioSystem::setBufferUpdateRate(float milliseconds)
{
	m_UpdateInterval = milliseconds;
}

AudioSystem::AudioSystem()
    : m_Context(nullptr)
    , m_Device(nullptr)
    , m_UpdateInterval(1000 * MILLISECONDS)
{
}

AudioSystem::~AudioSystem()
{
	shutDown();
}
