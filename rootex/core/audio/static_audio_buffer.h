#pragma once

#include "audio_buffer.h"

class StaticAudioBuffer : public AudioBuffer
{
	ALuint m_BufferID;

	void initializeBuffers() override;
	void destroyBuffers() override;

public:
	StaticAudioBuffer(AudioResourceFile* audioFile);
	~StaticAudioBuffer();

	ALuint& getBuffer();
	int getBufferSize();
};
