#pragma once

#include "audio_buffer.h"

/// An audio buffer that is sent entirely at once to the audio card. Preferable for smaller audio files. Larger files may be slow to start.
class StaticAudioBuffer : public AudioBuffer
{
	ALuint m_BufferID;

	void initializeBuffers() override;
	void destroyBuffers() override;

public:
	StaticAudioBuffer(AudioResourceFile* audioFile);
	~StaticAudioBuffer();

	ALuint& getBuffer();
};
