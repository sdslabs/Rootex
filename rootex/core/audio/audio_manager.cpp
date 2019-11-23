#include "audio_manager.h"

AudioManager* AudioManager::s_Singleton = nullptr;

void AudioManager::SetSingleton(AudioManager* singleton)
{
	s_Singleton = singleton;
}

AudioManager* AudioManager::GetSingleton()
{
	return s_Singleton;
}

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
}
