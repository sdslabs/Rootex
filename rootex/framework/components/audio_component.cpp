#include "audio_component.h"

AudioComponent::AudioComponent(bool playOnStart, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance)
    : m_IsPlayOnStart(playOnStart)
    , m_AttenuationModel(model)
    , m_RolloffFactor(rolloffFactor)
    , m_ReferenceDistance(referenceDistance)
    , m_MaxDistance(maxDistance)
{
}

AudioSource* getAudioSource()
{
	return nullptr;
}

JSON::json AudioComponent::getJSON() const
{
	JSON::json j;

	j["attenuationModel"] = m_AttenuationModel;
	j["rollOffFactor"] = m_RolloffFactor;
	j["referenceDistance"] = m_ReferenceDistance;
	j["maxDistance"] = m_MaxDistance;
}

bool AudioComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();

	if (m_TransformComponent)
	{
		getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
		getAudioSource()->setModel(m_AttenuationModel);
		getAudioSource()->setRollOffFactor(m_RolloffFactor);
		getAudioSource()->setReferenceDistance(m_ReferenceDistance);
		getAudioSource()->setMaxDistance(m_MaxDistance);
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
}