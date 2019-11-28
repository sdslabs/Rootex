#include "audio_static_buffer.h"

#include "audio_system.h"
#include "resource_data.h"

void AudioStaticBuffer::initializeBuffers()
{
	PANIC(m_AudioFile->getType() != ResourceFile::Type::WAV, "AudioSystem: Trying to load a non-WAV file in a sound buffer");

	AL_CHECK(m_BufferID = alutCreateBufferFromFileImage(
		m_AudioFile->getData()->getRawData()->getRawDataPointer(), 
		m_AudioFile->getData()->getRawData()->getSize()));

	AL_CHECK(alGetBufferi(m_BufferID, AL_FREQUENCY, &m_Frequency));
	AL_CHECK(alGetBufferi(m_BufferID, AL_BITS, &m_BitDepth));
	AL_CHECK(alGetBufferi(m_BufferID, AL_CHANNELS, &m_Channels));
	AL_CHECK(alGetBufferi(m_BufferID, AL_SIZE, &m_BufferSize));
}

void AudioStaticBuffer::destroyBuffers()
{
	AL_CHECK(alDeleteBuffers(1, &m_BufferID));
}

AudioStaticBuffer::AudioStaticBuffer(Ref<ResourceFile> audioFile)
    : AudioBuffer(audioFile)
{
	initializeBuffers();
}

AudioStaticBuffer::~AudioStaticBuffer()
{
	destroyBuffers();
}

int AudioStaticBuffer::getFrequency()
{
	return m_Frequency;
}

int AudioStaticBuffer::getBitDepth()
{
	return m_BitDepth;
}

int AudioStaticBuffer::getChannels()
{
	return m_Channels;
}

ALuint AudioStaticBuffer::getBuffer()
{
	return m_BufferID;
}

int AudioStaticBuffer::getBufferSize()
{
	return m_BufferSize;
}
