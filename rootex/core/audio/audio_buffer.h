#pragma once

#include "core/resource_file.h"

typedef unsigned int ALuint;

/// Interface for an audio buffer that is used inside an AudioSource.
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

	AudioResourceFile* getAudioFile();
};
