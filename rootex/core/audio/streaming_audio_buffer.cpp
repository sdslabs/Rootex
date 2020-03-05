#include "streaming_audio_buffer.h"

#include "audio_system.h"
#include "resource_data.h"
#include "resource_file.h"
#include "audio_utils.h"

void StreamingAudioBuffer::initializeBuffers()
{
	PANIC(m_AudioFile->getType() != ResourceFile::Type::WAV, "AudioSystem: Trying to load a non-WAV file in a sound buffer");

	ResourceData* fileStream = m_AudioFile->getData();
	FileBuffer* fileBuffer = fileStream->getRawData();

	AL_CHECK(alGenBuffers(BUFFER_COUNT, m_Buffers));

	ALsizei blockAlign = m_AudioFile->getChannels() * (m_AudioFile->getBitDepth() / 8.0);

	m_BufferSize = m_AudioFile->getAudioDataSize() / BUFFER_COUNT;
	m_BufferSize -= (m_BufferSize % blockAlign);
	m_BufferCursor = m_AudioFile->getData()->getRawData()->data();
	m_BufferEnd = m_BufferCursor + m_AudioFile->getAudioDataSize();

	int i = 0;
	while (i < MAX_BUFFER_QUEUE_LENGTH)
	{
		if (m_BufferCursor > m_AudioFile->getData()->getRawData()->data() + m_AudioFile->getAudioDataSize())
		{
			break;
		}
		AL_CHECK(alBufferData(
		    m_Buffers[i],
		    m_AudioFile->getFormat(),
		    (const ALvoid*)m_BufferCursor,
		    (ALsizei)m_BufferSize,
		    m_AudioFile->getFrequency()));

		m_BufferCursor += m_BufferSize;
		i++;
	}

	m_BufferQueueLength = i;
}

void StreamingAudioBuffer::destroyBuffers()
{
	AL_CHECK(alDeleteBuffers(BUFFER_COUNT, m_Buffers));
}

void StreamingAudioBuffer::loadNewBuffers(int count, bool isLooping)
{
	for (int i = 0; i < count; i++)
	{
		m_BufferEnd = m_BufferCursor + m_BufferSize;

		if (m_BufferCursor == m_AudioFile->getData()->getRawData()->data() + m_AudioFile->getAudioDataSize()) // Data has exhausted
		{
			if (isLooping) // Re-queue if looping
			{
				m_BufferCursor = m_AudioFile->getData()->getRawData()->data();
			}
			else
			{
				break;
			}
		}

		if (m_BufferEnd >= m_AudioFile->getData()->getRawData()->data() + m_AudioFile->getAudioDataSize()) // Data not left enough to entirely fill the next buffer
		{
			m_BufferEnd = m_AudioFile->getData()->getRawData()->data() + m_AudioFile->getAudioDataSize(); // Only take what you can
		}

		AL_CHECK(alBufferData(
		    m_Buffers[i],
		    m_AudioFile->getFormat(),
		    m_BufferCursor,
		    m_BufferEnd - m_BufferCursor,
		    m_AudioFile->getFrequency()));

		m_BufferCursor = m_BufferEnd;
	}
}

StreamingAudioBuffer::StreamingAudioBuffer(AudioResourceFile* audioFile)
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

int StreamingAudioBuffer::getBufferQueueLength()
{
	return m_BufferQueueLength;
}
