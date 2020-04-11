#include "audio_component.h"

AudioComponent::AudioComponent(bool playOnStart)
    : m_IsPlayOnStart(playOnStart)
{
}



bool AudioComponent::setup()
{
	
	// get the transform component 
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();

	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());


	return true;
}


void AudioComponent::update() 
{
	// get the transform component
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();

	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
}