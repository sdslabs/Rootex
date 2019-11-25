#include "audio_source.h"

AudioSource::AudioSource(AudioBuffer* audio)
{
	m_SourceID = AudioSystem::GetSingleton()->makeSource();
	attach(audio);
}

AudioSource::~AudioSource()
{
	AudioSystem::GetSingleton()->deleteSource(this);
}

void AudioSource::attach(AudioBuffer* audio)
{
	AudioSystem::GetSingleton()->attach(this, audio);
}

void AudioSource::play()
{
	AudioSystem::GetSingleton()->play(this);
}
