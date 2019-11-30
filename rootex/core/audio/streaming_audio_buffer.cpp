#include "streaming_audio_buffer.h"

#include "audio_system.h"
#include "resource_data.h"
#include "resource_file.h"

void StreamingAudioBuffer::initializeBuffers()
{
	PANIC(m_AudioFile->getType() != ResourceFile::Type::WAV, "AudioSystem: Trying to load a non-WAV file in a sound buffer");

	ResourceData* fileStream = m_AudioFile->getData();
	FileBuffer* fileBuffer = fileStream->getRawData();

	float frequencyFloat;
	ALUT_CHECK(m_DecompressedAudioBuffer = (const char*)alutLoadMemoryFromFile(m_AudioFile->getPath().c_str(), &m_Format, &m_AudioDataSize, &frequencyFloat));
	m_Frequency = (ALsizei)frequencyFloat;

	AL_CHECK(alGenBuffers(BUFFER_COUNT, m_Buffers));
	
	m_Channels = 2;
	m_BitDepth = 16;
	ALsizei blockAlign = m_Channels * (m_BitDepth / 8.0);
	
	m_BufferSize = m_AudioDataSize / BUFFER_COUNT;
	m_BufferSize -= (m_BufferSize % blockAlign);
	m_BufferCursor = m_DecompressedAudioBuffer;
	m_BufferEnd = m_BufferCursor + m_AudioDataSize;

	int i = 0;
	while (i < BUFFER_COUNT)
	{
		AL_CHECK(alBufferData(m_Buffers[i], m_Format, (const ALvoid*)m_BufferCursor, (ALsizei)m_BufferSize, m_Frequency));
		
		m_BufferCursor += m_BufferSize;
		i++;
	}
}

void StreamingAudioBuffer::destroyBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		AL_CHECK(alDeleteBuffers(1, &m_Buffers[i]));
	}

	for (int i = 0; i < m_AudioDataSize; i++)
	{
		delete (m_DecompressedAudioBuffer + i);
	}
}

void StreamingAudioBuffer::loadNewBuffers(int count, bool isLooping)
{
	for (int i = 0; i < count; i++)
	{
		m_BufferEnd = m_BufferCursor + m_BufferSize;

		if (m_BufferCursor == m_DecompressedAudioBuffer + m_AudioDataSize) // Data has exhausted
		{
			if (isLooping) // Re-queue if looping
			{
				m_BufferCursor = m_DecompressedAudioBuffer;
			}
		}
		
		if (m_BufferEnd >= m_DecompressedAudioBuffer + m_AudioDataSize) // Data not left enough to entirely fill the next buffer
		{
			m_BufferEnd = m_DecompressedAudioBuffer + m_AudioDataSize; // Only take what you can
		}

		AL_CHECK(alBufferData(m_Buffers[i], m_Format, m_BufferCursor, m_BufferEnd - m_BufferCursor, m_Frequency));
		m_BufferCursor = m_BufferEnd;	
	}
}

StreamingAudioBuffer::StreamingAudioBuffer(Ref<ResourceFile> audioFile)
    : AudioBuffer(audioFile)
{
	initializeBuffers();
}

StreamingAudioBuffer::~StreamingAudioBuffer()
{
	destroyBuffers();
}

ALuint* StreamingAudioBuffer::getBuffers()
{
	return m_Buffers;
}

int StreamingAudioBuffer::getFrequency()
{
	return m_Frequency;
}

int StreamingAudioBuffer::getBitDepth()
{
	return m_BitDepth;
}

int StreamingAudioBuffer::getChannels()
{
	return m_Channels;
}
