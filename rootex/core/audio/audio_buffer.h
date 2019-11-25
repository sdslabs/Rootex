#pragma once

#include "core/resource_file.h"

typedef unsigned int ALuint;

class AudioBuffer
{
	Ref<ResourceFile> m_AudioFile;
	float m_Frequency;
	bool m_Looping;
	ALuint m_BufferID;

	friend class AudioSystem;

public:
	AudioBuffer(Ref<ResourceFile> audioFile, float frequency, bool looping);
	~AudioBuffer();

	ResourceFile* getAudioFile();
	float getFrequency();
	bool isLooping();
};
