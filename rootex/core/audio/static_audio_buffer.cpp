#include "static_audio_buffer.h"

#include "audio_system.h"
#include "resource_data.h"

void StaticAudioBuffer::initializeBuffers()
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

void StaticAudioBuffer::destroyBuffers()
{
	AL_CHECK(alDeleteBuffers(1, &m_BufferID));
}

StaticAudioBuffer::StaticAudioBuffer(Ref<ResourceFile> audioFile)
    : AudioBuffer(audioFile)
{
	initializeBuffers();
}

StaticAudioBuffer::~StaticAudioBuffer()
{
	destroyBuffers();
}

int StaticAudioBuffer::getFrequency()
{
	return m_Frequency;
}

int StaticAudioBuffer::getBitDepth()
{
	return m_BitDepth;
}

int StaticAudioBuffer::getChannels()
{
	return m_Channels;
}

ALuint StaticAudioBuffer::getBuffer()
{
	return m_BufferID;
}

int StaticAudioBuffer::getBufferSize()
{
	return m_BufferSize;
}
