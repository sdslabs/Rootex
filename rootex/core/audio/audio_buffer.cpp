#include "audio_buffer.h"
#include "audio_system.h"

AudioBuffer::AudioBuffer(Ref<ResourceFile> audioFile, float frequency, bool looping)
    : m_AudioFile(audioFile)
    , m_Frequency(frequency)
    , m_Looping(looping)
{
	AudioSystem::GetSingleton()->m_ActiveAudioBuffers.push_back(this);
	
	m_BufferID = AudioSystem::GetSingleton()->makeBuffer(audioFile.get());
}

AudioBuffer::~AudioBuffer()
{
	AudioSystem::GetSingleton()->destroyBuffer(this);

	AudioSystem::GetSingleton()->m_ActiveAudioBuffers.erase(
	    std::find(
	        AudioSystem::GetSingleton()->m_ActiveAudioBuffers.begin(), 
			AudioSystem::GetSingleton()->m_ActiveAudioBuffers.end(), 
			this)
	);
}

ResourceFile* AudioBuffer::getAudioFile()
{
	return m_AudioFile.get();
}

float AudioBuffer::getFrequency()
{
	return m_Frequency;
}

bool AudioBuffer::isLooping()
{
	return m_Looping;
}
