#pragma once

#include "common/common.h"

#include "vendor/OpenAL/include/al.h"
#include "vendor/OpenAL/include/alc.h"
#include "vendor/OpenAL/include/alut.h"

#include "system.h"

#ifndef ALUT_CHECK
#ifdef _DEBUG
#define ALUT_CHECK(alutFunction)                                        \
	do                                                                  \
	{                                                                   \
		alutFunction;                                                   \
		AudioSystem::CheckALUTError(#alutFunction, __FILE__, __LINE__); \
	} while (0)
#else
#define ALUT_CHECK(alutFunction) alutFunction
#endif
#endif

#ifndef AL_CHECK
#ifdef _DEBUG
#define AL_CHECK(alFunction)                                        \
	do                                                              \
	{                                                               \
		alFunction;                                                 \
		AudioSystem::CheckALError(#alFunction, __FILE__, __LINE__); \
	} while (0)
#else
#define AL_CHECK(alFunction) alFunction
#endif
#endif

class AudioBuffer;
class StaticAudioBuffer;
class StreamingAudioBuffer;

class AudioSource;
class StreamingAudioSource;

class ResourceFile;

/// System encapsulating OpenAL error checkers and getters.
class AudioSystem : public System
{
	unsigned int m_UpdateIntervalMilliseconds;
	ALCdevice* m_Device;
	ALCcontext* m_Context;
	int m_ListenerID;

	AudioSystem();
	AudioSystem(AudioSystem&) = delete;
	~AudioSystem();

public:
	static AudioSystem* GetSingleton();

	/// Returns error string corresponding to AL error codes.
	static String GetALErrorString(int errID);
	/// Returns error string corresponding to ALC error codes.
	static String GetALCErrorString(int errID);
	/// Wrapper over alGetError function.
	static void CheckALError(const char* msg, const char* fname, int line);
	/// Wrapper over alcGetError function.
	static void CheckALCError(const char* msg, const char* fname, int line);
	/// Wrapper over alutGetError function.
	static void CheckALUTError(const char* msg, const char* fname, int line);

	void setBufferUpdateRate(float milliseconds);

	// set listener in openAL
	static void setListener(int listenerID);
	bool initialize();
	void begin();
	void update();
	void shutDown();
};
