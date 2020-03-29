#include "audio_buffer.h"

AudioBuffer::AudioBuffer(AudioResourceFile* audioFile)
    : m_AudioFile(audioFile)
{
}

AudioBuffer::~AudioBuffer()
{
}

AudioResourceFile* AudioBuffer::getAudioFile()
{
	return m_AudioFile;
}
