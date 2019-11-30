#pragma once

#include "audio_buffer.h"

class StaticAudioBuffer : public AudioBuffer
{
	ALuint m_BufferID;
	int m_BufferSize;

	int m_Frequency;
	int m_BitDepth;
	int m_Channels;

	void initializeBuffers() override;
	void destroyBuffers() override;

public:
	StaticAudioBuffer(Ref<ResourceFile> audioFile);
	~StaticAudioBuffer();

	ALuint getBuffer();
	int getBufferSize();

	int getFrequency() override;
	int getBitDepth() override;
	int getChannels() override;
};
