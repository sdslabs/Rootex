#include "audio_component.h"

AudioComponent::AudioComponent(bool playOnStart)
    : m_IsPlayOnStart(playOnStart)
{
}



bool AudioComponent::setup()
{
	
	// set the position of the audio source using the co-ordinates provided by transformComponent
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	return true;
}


void AudioComponent::update() 
{
	// set the position of the audio source using the co-ordinates provided by transformComponent
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
}