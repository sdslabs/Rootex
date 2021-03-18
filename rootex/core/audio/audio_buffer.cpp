#include "audio_buffer.h"

AudioBuffer::AudioBuffer(Ref<AudioResourceFile> audioFile)
    : m_AudioFile(audioFile)
{
}

AudioBuffer::~AudioBuffer()
{
}

AudioResourceFile* AudioBuffer::getAudioFile()
{
	return m_AudioFile.get();
}
