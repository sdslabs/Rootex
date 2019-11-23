#pragma once

#include "common/common.h"
#include "core/audio/audio_manager.h"
#include "vendor/OpenAL/include/al.h"
#include "vendor/OpenAL/include/alc.h"

class AudioManagerOpenAL : public AudioManager
{
	ALCdevice* m_DeviceAL;
	ALCcontext* m_ContextAL;

	static String GetOpenALErrorString(int errID);

public:
	static void CheckOpenALError(const char* msg, const char* fname, int line);

	bool initialize() override;
	void shutDown() override;
};
