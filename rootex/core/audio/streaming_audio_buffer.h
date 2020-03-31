#pragma once

#define BUFFER_COUNT 50
#define MAX_BUFFER_QUEUE_LENGTH BUFFER_COUNT / 25

#include "audio_buffer.h"
#include "framework/systems/audio_system.h"

class StreamingAudioBuffer : public AudioBuffer
{
	ALuint m_Buffers[BUFFER_COUNT];
	ALsizei m_BufferSize;

	const char* m_BufferCursor;
	int m_BufferQueueLength;
	const char* m_BufferEnd;

	void initializeBuffers() override;
	void destroyBuffers() override;

public:
	StreamingAudioBuffer(AudioResourceFile* audioFile);
	~StreamingAudioBuffer();

	void loadNewBuffers(int count, bool isLooping);

	ALuint* getBuffers();
	int getBufferQueueLength();
};
