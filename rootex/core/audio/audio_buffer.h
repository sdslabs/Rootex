#pragma once

#include "core/resource_files/audio_resource_file.h"

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
