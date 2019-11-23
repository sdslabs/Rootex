#include "openal_audio_manager.h"

#ifndef AL_CHECK
#ifdef _DEBUG
#define AL_CHECK(m_Msg)                               \
	do                                                \
	{                                                 \
		m_Msg;                                        \
		AudioManagerOpenAL::CheckOpenALError(#m_Msg, __FILE__, __LINE__); \
	} while (0);
#else
#define AL_CHECK(stmt) stmt
#endif
#endif

String AudioManagerOpenAL::GetOpenALErrorString(int errID)
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

inline void AudioManagerOpenAL::CheckOpenALError(const char* msg, const char* fname, int line)
{
	ALenum err = alGetError();
	if (err != AL_NO_ERROR)
	{
		ERR("OpenAL error (" + std::to_string(err) + "): " + GetOpenALErrorString(err) + " at " + String(fname) + ":" + std::to_string(line));
	}
}

bool AudioManagerOpenAL::initialize()
{
	alGetError();

	m_DeviceAL = alcOpenDevice(nullptr);

	if (m_DeviceAL == nullptr)
	{
		ERR("OpenALAudioManager: Failed to initialize OpenAL device");
		return false;
	}

	m_ContextAL = alcCreateContext(m_DeviceAL, nullptr); 

	AL_CHECK(alcMakeContextCurrent(m_ContextAL));

	return true;
}

void AudioManagerOpenAL::shutDown()
{
}
