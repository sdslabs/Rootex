#pragma once

#include "core/resource_file.h"

typedef unsigned int ALuint;

class AudioBuffer
{
protected:
	AudioResourceFile* m_AudioFile;

	AudioBuffer(AudioResourceFile* audioFile);

	virtual void initializeBuffers() = 0;
	virtual void destroyBuffers() = 0;

public:
	AudioBuffer(AudioBuffer&) = default;
	virtual ~AudioBuffer();

	ResourceFile* getAudioFile();
};
