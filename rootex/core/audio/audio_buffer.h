#pragma once

#include "core/resource_file.h"

typedef unsigned int ALuint;

class AudioBuffer
{
protected:
	Ref<ResourceFile> m_AudioFile;

	AudioBuffer(Ref<ResourceFile> audioFile);

	virtual void initializeBuffers() = 0;
	virtual void destroyBuffers() = 0;

public:
	AudioBuffer(AudioBuffer&) = default;
	virtual ~AudioBuffer();

	ResourceFile* getAudioFile();
	
	virtual int getFrequency() = 0;
	virtual int getBitDepth() = 0;
	virtual int getChannels() = 0;
};
