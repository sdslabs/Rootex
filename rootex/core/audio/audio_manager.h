#pragma once

#include "common/types.h"
#include "core/resource_file.h"

class IAudioBuffer;

class AudioManager
{
	static AudioManager* s_Singleton;

public:
	static void SetSingleton(AudioManager* singleton);
	static AudioManager* GetSingleton();

	virtual bool initialize() = 0;
	virtual void shutDown() = 0;

protected:
	explicit AudioManager();
	~AudioManager();
};
