#pragma once

#define BUFFER_COUNT 5

#include "audio_buffer.h"
#include "audio_system.h"

class StreamingAudioBuffer : public AudioBuffer
{
	ALuint m_Buffers[BUFFER_COUNT];

	float m_Frequency;
	int m_BitDepth;
	int m_Channels;
	int m_BufferSize;
	
	const char* m_DecompressedAudioBuffer;
	ALsizei m_AudioDataSize;
	ALenum m_Format;

	const char* m_BufferCursor;
	const char* m_BufferEnd;

	void initializeBuffers() override;
	void destroyBuffers() override;
	
public:
	StreamingAudioBuffer(Ref<ResourceFile> audioFile);
	~StreamingAudioBuffer();
	
	void loadNewBuffers(int count, bool isLooping);

	ALuint* getBuffers();
	int getFrequency() override;
	int getBitDepth() override;
	int getChannels() override;
};
