#include "audio_buffer.h"
#include "audio_system.h"

AudioBuffer::AudioBuffer(Ref<ResourceFile> audioFile)
    : m_AudioFile(audioFile)
{
}

AudioBuffer::~AudioBuffer()
{
}

ResourceFile* AudioBuffer::getAudioFile()
{
	return m_AudioFile.get();
}
