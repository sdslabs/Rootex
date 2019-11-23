#pragma once

#include "common/common.h"
#include "core/audio/audio_manager.h"
#include "vendor/OpenAL/include/al.h"
#include "vendor/OpenAL/include/alc.h"

class AudioSystem
{
	ALCdevice* m_Device;
	ALCcontext* m_Context;

	bool initialize();
	void shutDown();

public:
	AudioSystem();
	AudioSystem(AudioSystem&) = delete;
	~AudioSystem();

	static AudioSystem* GetSingleton();
	
	static String GetOpenALErrorString(int errID);
	static void CheckOpenALError(const char* msg, const char* fname, int line);
};
