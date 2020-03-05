#pragma once

#include "common/common.h"

class AudioBuffer;
class StaticAudioBuffer;
class StreamingAudioBuffer;
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
class AudioSource;
class StreamingAudioSource;
class ResourceFile;

class AudioSystem
{
	unsigned int m_UpdateIntervalMilliseconds;
	ALCdevice* m_Device;
	ALCcontext* m_Context;
	Vector<AudioSource*> m_ActiveAudioSources;

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
	void shutDown();
};
