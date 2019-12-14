#pragma once

#include "common/common.h"

#include "vendor/OpenAL/include/al.h"
#include "vendor/OpenAL/include/alc.h"
#include "vendor/OpenAL/include/alut.h"

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

class AudioSystem
{
	const int MILLISECONDS = 1;
	int m_UpdateInterval;
	ALCdevice* m_Device;
	ALCcontext* m_Context;
	Vector<AudioSource*> m_ActiveAudioSources;

	void shutDown();

public:
	AudioSystem();
	AudioSystem(AudioSystem&) = delete;
	~AudioSystem();

	static AudioSystem* GetSingleton();
	static String GetALErrorString(int errID);
	static String GetALCErrorString(int errID);
	static void CheckALError(const char* msg, const char* fname, int line);
	static void CheckALCError(const char* msg, const char* fname, int line);
	static void CheckALUTError(const char* msg, const char* fname, int line);

	void registerInstance(AudioSource* audio);
	void deregisterInstance(AudioSource* audio);

	void setBufferUpdateRate(float milliseconds);

	bool initialize();
	void update();
};
