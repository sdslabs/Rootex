#include "audio_component.h"

AudioComponent::AudioComponent(bool playOnStart, int rolloffFactor, int referenceDistance, int maxDistance)
    : m_IsPlayOnStart(playOnStart)
{
	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	getAudioSource()->setModel(AudioSource::AttenuationModel::Exponential);
	getAudioSource()->setRollOffFactor(rolloffFactor);
	getAudioSource()->setReferenceDistance(referenceDistance);
	getAudioSource()->setMaxDistance(maxDistance);
}

bool AudioComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();

	if (m_TransformComponent)
	{
		getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
		getAudioSource()->setModel(AudioSource::AttenuationModel::Exponential);
		getAudioSource()->setRollOffFactor(1);
		getAudioSource()->setReferenceDistance(1);
		getAudioSource()->setMaxDistance(100);
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

void AudioComponent::update()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	getAudioSource()->setModel(AudioSource::AttenuationModel::Exponential);
}