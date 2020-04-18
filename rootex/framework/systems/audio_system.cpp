#include "audio_system.h"

#include <chrono>
#include <thread>

#include "components/audio_component.h"
#include "core/audio/audio_source.h"
#include "core/audio/static_audio_buffer.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/resource_data.h"

String AudioSystem::GetALErrorString(int errID)
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

String AudioSystem::GetALCErrorString(int errID)
{
	switch (errID)
	{
	case ALC_NO_ERROR:
		return "OK";
	case ALC_INVALID_DEVICE:
		return "Invalid Device";
	case ALC_INVALID_CONTEXT:
		return "Invalid Context";
	case ALC_INVALID_ENUM:
		return "Invalid Enu,";
	case ALC_INVALID_VALUE:
		return "Invalid Value";
	case ALC_OUT_OF_MEMORY:
		return "Out of Memory";
	}
	return "Unknown Error: " + std::to_string(errID);
}

void AudioSystem::CheckALError(const char* msg, const char* fname, int line)
{
	ALenum err = alGetError();
	if (err != AL_NO_ERROR)
	{
		ERR("OpenAL error (" + std::to_string(err) + "): " + GetALErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

void AudioSystem::CheckALCError(const char* msg, const char* fname, int line)
{
	ALCenum err = alcGetError(GetSingleton()->m_Device);
	if (err != ALC_NO_ERROR)
	{
		ERR("OpenALC error (" + std::to_string(err) + "): " + GetALCErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
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
	if (!alutInit(NULL, NULL))
	{
		ERR("AudioSystem: AL, ALC, ALUT failed to initialize");
		return false;
	}

	return true;
}

void AudioSystem::begin()
{
	AudioComponent* audioComponent = nullptr;
	for (Component* component : s_Components[AudioComponent::s_ID])
	{
		audioComponent = (AudioComponent*)component;
		if (audioComponent->isPlayOnStart())
		{
			audioComponent->getAudioSource()->play();
		}
	}
}

void AudioSystem::update()
{
	AudioComponent* audioComponent = nullptr;
	for (Component* component : s_Components[AudioComponent::s_ID])
	{
		audioComponent = (AudioComponent*)component;
		audioComponent->getAudioSource()->queueNewBuffers();
		audioComponent->update();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(m_UpdateIntervalMilliseconds));
}

AudioSystem* AudioSystem::GetSingleton()
{
	static AudioSystem singleton;
	return &singleton;
}

void AudioSystem::shutDown()
{
	AudioComponent* audioComponent = nullptr;
	for (Component* component : s_Components[AudioComponent::s_ID])
	{
		audioComponent = (AudioComponent*)component;
		audioComponent->getAudioSource()->stop();
	}
	alutExit();
}

void AudioSystem::setBufferUpdateRate(float milliseconds)
{
	m_UpdateIntervalMilliseconds = milliseconds;
}

AudioSystem::AudioSystem()
    : m_Context(nullptr)
    , m_Device(nullptr)
    , m_UpdateIntervalMilliseconds(0)
{
	AL_CHECK(alListener3f(AL_POSITION, 0, 0, 0));
	AL_CHECK(alListener3f(AL_VELOCITY, 0, 0, 0));
}

AudioSystem::~AudioSystem()
{
}
