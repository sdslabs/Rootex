#include "audio_component.h"

AudioComponent::AudioComponent(bool playOnStart, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance)
    : m_IsPlayOnStart(playOnStart)
    , m_IsAttenuated(attenuation)
    , m_AttenuationModel(model)
    , m_RolloffFactor(rolloffFactor)
    , m_ReferenceDistance(referenceDistance)
    , m_MaxDistance(maxDistance)
    , m_TransformComponent(nullptr)
{
}

bool AudioComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			status = false;
		}
	}
	return status;
	//m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();

	//if (m_TransformComponent)
	//{
	//AudioComponent::getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	//AudioComponent::getAudioSource()->setModel(m_AttenuationModel);
	//AudioComponent::getAudioSource()->setRollOffFactor(m_RolloffFactor);
	//AudioComponent::getAudioSource()->setReferenceDistance(m_ReferenceDistance);
	//AudioComponent::getAudioSource()->setMaxDistance(m_MaxDistance);
	//return true;
	//	}
}

JSON::json AudioComponent::getJSON() const
{
	JSON::json j;

	j["isAttenuated"] = m_IsAttenuated;
	j["attenuationModel"] = m_AttenuationModel;
	j["rollOffFactor"] = m_RolloffFactor;
	j["referenceDistance"] = m_ReferenceDistance;
	j["maxDistance"] = m_MaxDistance;

	return j;
}

AudioSource* AudioComponent::getAudioSource()
{
	return nullptr;
}

void AudioComponent::update()
{
	//m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	//AudioComponent::getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AudioComponent::draw()
{
	ImGui::Checkbox("Attenuation", &m_IsAttenuated);
}
#endif // ROOTEX_EDITOR
