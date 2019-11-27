#pragma once

#include "common/common.h"
#include "core/audio/audio_buffer.h"
#include "core/audio/audio_source.h"
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
#define AL_CHECK(alFunction)                                            \
	do                                                                  \
	{                                                                   \
		alFunction;                                                     \
		AudioSystem::CheckOpenALError(#alFunction, __FILE__, __LINE__); \
	} while (0)
#else
#define AL_CHECK(alFunction) alFunction
#endif
#endif

class AudioSource;

class AudioSystem
{
	Vector<AudioBuffer*> m_ActiveAudioBuffers;

	const unsigned int NUM_BUFFERS = 100;

	ALCdevice* m_Device;
	ALCcontext* m_Context;
	Vector<ALuint> m_Buffers;

	void shutDown();

	ALuint makeBuffer(ResourceFile* audioFile);
	void destroyBuffer(AudioBuffer* buffer);
	ALuint makeSource();
	void attach(AudioSource* source, AudioBuffer* audio);
	void deleteSource(AudioSource* source);

	friend class AudioBuffer;
	friend class AudioSource;

public:
	AudioSystem();
	AudioSystem(AudioSystem&) = delete;
	~AudioSystem();

	static AudioSystem* GetSingleton();
	static String GetOpenALErrorString(int errID);
	static void CheckOpenALError(const char* msg, const char* fname, int line);
	static void CheckALUTError(const char* msg, const char* fname, int line);

	bool initialize();
	void play(AudioSource* source);
};
