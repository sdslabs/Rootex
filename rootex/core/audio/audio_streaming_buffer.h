#pragma once

#define MAX_BUFFER_SIZE 32000
#define BUFFER_COUNT 5

#include "audio_buffer.h"
#include "audio_system.h"

class AudioStreamingBuffer : public AudioBuffer
{
	ALuint* m_Buffers;

	int m_Frequency;
	int m_BitDepth;
	int m_Channels;
	
	const char* m_DecompressedFileBuffer;
	ALsizei m_FileBufferSize;
	ALenum m_Format;

	const char* m_BufferCursor;
	const char* m_BufferEnd;

	void initializeBuffers() override;
	void destroyBuffers() override;
	
public:
	AudioStreamingBuffer(Ref<ResourceFile> audioFile);
	~AudioStreamingBuffer();
	
	void loadNewBuffers(int count, bool isLooping);

	ALuint* getBuffers();
	int getFrequency() override;
	int getBitDepth() override;
	int getChannels() override;
};
