#include "audio_streaming_buffer.h"

#include "audio_system.h"
#include "resource_data.h"
#include "resource_file.h"

void AudioStreamingBuffer::initializeBuffers()
{
	ResourceData* fileStream = m_AudioFile->getData();
	FileBuffer* fileBuffer = fileStream->getRawData();

	AL_CHECK(alGenBuffers(BUFFER_COUNT, m_Buffers));

	{
		float frequencyInt;
		ALUT_CHECK(m_DecompressedFileBuffer = (const char*)alutLoadMemoryFromFileImage(
			fileBuffer->m_Buffer.data(),
			fileBuffer->m_Buffer.size(),
			&m_Format,
			&m_FileBufferSize,
			&frequencyInt));
		m_Frequency = (int)frequencyInt;
	}

	AL_CHECK(alGetBufferi(m_Buffers[0], AL_FREQUENCY, &m_Frequency));
	AL_CHECK(alGetBufferi(m_Buffers[0], AL_BITS, &m_BitDepth));
	AL_CHECK(alGetBufferi(m_Buffers[0], AL_CHANNELS, &m_Channels));

	m_BufferCursor = m_DecompressedFileBuffer;
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_BufferEnd = m_BufferCursor + MAX_BUFFER_SIZE;
		if (m_BufferEnd >= m_DecompressedFileBuffer + m_FileBufferSize)
		{
			m_BufferEnd = m_DecompressedFileBuffer + m_FileBufferSize;
		}

		AL_CHECK(alBufferData(m_Buffers[i], m_Format, m_BufferCursor, m_BufferEnd - m_BufferCursor, m_Frequency));
		m_BufferCursor = m_BufferEnd;
	}
}

void AudioStreamingBuffer::destroyBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		AL_CHECK(alDeleteBuffers(1, &m_Buffers[i]));
	}
}

void AudioStreamingBuffer::loadNewBuffers(int count, bool isLooping)
{
	for (int i = 0; i < count; i++)
	{
		m_BufferEnd = m_BufferCursor + MAX_BUFFER_SIZE;

		if (m_BufferCursor == m_DecompressedFileBuffer + m_FileBufferSize) // Data has exhausted
		{
			if (isLooping) // Re-queue if looping
			{
				m_BufferCursor = m_DecompressedFileBuffer;
			}
		}
		
		if (m_BufferEnd >= m_DecompressedFileBuffer + m_FileBufferSize) // Data not left enough to entirely fill the next buffer
		{
			m_BufferEnd = m_DecompressedFileBuffer + m_FileBufferSize; // Only take what you can
		}

		AL_CHECK(alBufferData(m_Buffers[i], m_Format, m_BufferCursor, m_BufferEnd - m_BufferCursor, m_Frequency));
		m_BufferCursor = m_BufferEnd;	
	}
}

AudioStreamingBuffer::AudioStreamingBuffer(Ref<ResourceFile> audioFile)
    : AudioBuffer(audioFile)
{
	initializeBuffers();
}

AudioStreamingBuffer::~AudioStreamingBuffer()
{
	destroyBuffers();
}

ALuint* AudioStreamingBuffer::getBuffers()
{
	return m_Buffers;
}

int AudioStreamingBuffer::getFrequency()
{
	return m_Frequency;
}

int AudioStreamingBuffer::getBitDepth()
{
	return m_BitDepth;
}

int AudioStreamingBuffer::getChannels()
{
	return m_Channels;
}
