
#include "audio_component.h"

AudioComponent::AudioComponent(bool playOnStart)
    : m_IsPlayOnStart(playOnStart)
{
}

bool AudioComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	getAudioSource()->setModel(1, 1, 1, "linear", false);

	return true;
}

void AudioComponent::update()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	getAudioSource()->setModel(1, 1, 1, "linear", false);
}