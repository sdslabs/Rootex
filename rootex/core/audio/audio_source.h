#pragma once

#include "audio_system.h"

class AudioSource
{
	AudioBuffer* m_Audio;
	ALuint m_SourceID;

	friend class AudioSystem;

public:
	AudioSource(AudioBuffer* audio);
	~AudioSource();

	void attach(AudioBuffer* audio);
	void play();
};
