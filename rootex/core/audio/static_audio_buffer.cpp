#include "static_audio_buffer.h"

#include "framework/systems/audio_system.h"

void StaticAudioBuffer::initializeBuffers()
{
	PANIC(m_AudioFile->getType() != ResourceFile::Type::Audio, "AudioSystem: Trying to load a non-WAV file in a sound buffer");

	AL_CHECK(alGenBuffers(1, &m_BufferID));
	AL_CHECK(alBufferData(
	    m_BufferID,
	    m_AudioFile->getFormat(),
	    m_AudioFile->getAudioData(),
	    m_AudioFile->getAudioDataSize(),
	    m_AudioFile->getFrequency()));
}

void StaticAudioBuffer::destroyBuffers()
{
	AL_CHECK(alDeleteBuffers(1, &m_BufferID));
}

StaticAudioBuffer::StaticAudioBuffer(AudioResourceFile* audioFile)
    : AudioBuffer(audioFile)
{
	initializeBuffers();
}

StaticAudioBuffer::~StaticAudioBuffer()
{
	destroyBuffers();
}

ALuint& StaticAudioBuffer::getBuffer()
{
	return m_BufferID;
}
