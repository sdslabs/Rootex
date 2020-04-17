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
		if (m_IsAttenuated)
		{
			getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
			getAudioSource()->setModel(m_AttenuationModel);
			getAudioSource()->setRollOffFactor(m_RolloffFactor);
			getAudioSource()->setReferenceDistance(m_ReferenceDistance);
			getAudioSource()->setMaxDistance(m_MaxDistance);
		}
	}
	return status;
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

void AudioComponent::update()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	if (m_IsAttenuated)
	{
		getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	}
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AudioComponent::draw()
{
	ImGui::Checkbox("Play on Start", &m_IsPlayOnStart);
	ImGui::Checkbox("Turn on Attenuation", &m_IsAttenuated);

	if (ImGui::BeginCombo("Attenutation Model", m_AttenuationModelName.c_str()))
	{
		if (ImGui::Selectable("Linear"))
		{
			m_AttenuationModel = AudioSource::AttenuationModel::Linear;
			m_AttenuationModelName = "Linear";
		}
		if (ImGui::Selectable("Linear Clamped"))
		{
			m_AttenuationModel = AudioSource::AttenuationModel::LinearClamped;
			m_AttenuationModelName = "Linear Clamped";
		}
		if (ImGui::Selectable("Inverse"))
		{
			m_AttenuationModel = AudioSource::AttenuationModel::Inverse;
			m_AttenuationModelName = "Inverse";
		}
		if (ImGui::Selectable(("Inverse Clamped")))
		{
			m_AttenuationModel = AudioSource::AttenuationModel::InverseClamped;
			m_AttenuationModelName = "Inverse Clamped";
		}
		if (ImGui::Selectable("Exponential"))
		{
			m_AttenuationModel = AudioSource::AttenuationModel::Exponential;
			m_AttenuationModelName = "Expoenential";
		}
		if (ImGui::Selectable(("Exponential Clamped")))
		{
			m_AttenuationModel = AudioSource::AttenuationModel::ExponentialClamped;
			m_AttenuationModelName = "Exponential Clamped";
		}
		ImGui::EndCombo();
	}

	ImGui::InputFloat("Reference Distance", &m_ReferenceDistance, 0, 100.0f);
	ImGui::InputFloat("Rolloff Ractor", &m_RolloffFactor, 0, 100.0f);
	ImGui::InputFloat("Max Distance", &m_MaxDistance, 0, 100.0f);
}

#endif // ROOTEX_EDITOR
