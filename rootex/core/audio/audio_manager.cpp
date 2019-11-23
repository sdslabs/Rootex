#include "audio_manager.h"

#ifndef AL_CHECK
#ifdef _DEBUG
#define AL_CHECK(alFunction)                                              \
	do                                                                    \
	{                                                                     \
		alFunction;                                                       \
		AudioSystem::CheckOpenALError(#alFunction, __FILE__, __LINE__);   \
	} while (0)
#else
#define AL_CHECK(alFunction) alFunction
#endif
#endif

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

bool AudioSystem::initialize()
{
	alGetError();

	m_Device = alcOpenDevice(nullptr);

	if (m_Device == nullptr)
	{
		ERR("OpenALAudioManager: Failed to initialize OpenAL device");
		return false;
	}

	m_Context = alcCreateContext(m_Device, nullptr);

	AL_CHECK(alcMakeContextCurrent(m_Context));

	return true;
}

AudioSystem* AudioSystem::GetSingleton()
{
	static AudioSystem singleton;

	return &singleton;
}

void AudioSystem::shutDown()
{
}

AudioSystem::AudioSystem()
    : m_Context(nullptr)
    , m_Device(nullptr)
{
	bool ok = initialize();

	PANIC(ok == false, "AudioSystem: System Initialization failed.");
}

AudioSystem::~AudioSystem()
{
	shutDown();
}
