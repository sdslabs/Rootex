#pragma once

/// Number of equal slices that an audio buffer is cut down into.
#define BUFFER_COUNT 50
/// Maximum number of buffers queued at once.
#define MAX_BUFFER_QUEUE_LENGTH BUFFER_COUNT / 25

#include "audio_buffer.h"
#include "framework/systems/audio_system.h"

/// An audio buffer that is streamed to the audio card instead of sent entirely at once. Allows faster startup for large audio files.
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
