#pragma once

#include "common/types.h"
#include "core/resource.h"
#include "vendor/OpenAL/include/al.h"

class IAudioBuffer;

class IAudioManager
{
public:
	virtual Ref<IAudioBuffer> createAudioBuffer(Ref<WAVResource> soundResource) = 0;

	virtual bool pauseAllStops() = 0;
	virtual bool stopAllSounds() = 0;

	virtual bool initialize() = 0;
	virtual void shutDown() = 0;

protected:
	explicit IAudioManager();
	~IAudioManager();
};
