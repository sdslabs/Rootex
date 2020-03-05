#include "audio_system.h"

#include <chrono>
#include <thread>

#include "audio_source.h"
#include "core/resource_data.h"
#include "static_audio_buffer.h"
#include "streaming_audio_buffer.h"
#include "audio_utils.h"

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

void AudioSystem::update()
{
	for (auto* source : m_ActiveAudioSources)
	{
		source->queueNewBuffers();
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
	for (auto& source : m_ActiveAudioSources)
	{
		source->stop();
	}
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

	WARN("AudioSystem: Tried to double deregister an AudioBuffer. Delete aborted.");
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
}

AudioSystem::~AudioSystem()
{
}
