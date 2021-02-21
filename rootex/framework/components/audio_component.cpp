#include "audio_component.h"

#include "systems/render_system.h"

AudioComponent::AudioComponent(bool playOnStart, bool isLooping, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance)
    : m_IsPlayOnStart(playOnStart)
    , m_IsAttenuated(attenuation)
    , m_IsLooping(isLooping)
    , m_AttenuationModel(model)
    , m_RolloffFactor(rolloffFactor)
    , m_ReferenceDistance(referenceDistance)
    , m_MaxDistance(maxDistance)
    , m_DependencyOnTransformComponent(this)
{
}

bool AudioComponent::setupData()
{
	if (m_IsAttenuated)
	{
		getAudioSource()->setPosition(m_TransformComponent->getParentAbsoluteTransform().Translation());
		getAudioSource()->setModel(m_AttenuationModel);
		getAudioSource()->setRollOffFactor(m_RolloffFactor);
		getAudioSource()->setReferenceDistance(m_ReferenceDistance);
		getAudioSource()->setMaxDistance(m_MaxDistance);
	}

	getAudioSource()->setLooping(m_IsLooping);
	return true;
}

JSON::json AudioComponent::getJSON() const
{
	JSON::json j;

	j["isAttenuated"] = m_IsAttenuated;
	j["isLooping"] = m_IsLooping;
	j["attenuationModel"] = m_AttenuationModel;
	j["rollOffFactor"] = m_RolloffFactor;
	j["referenceDistance"] = m_ReferenceDistance;
	j["maxDistance"] = m_MaxDistance;

	return j;
}

void AudioComponent::update()
{
	if (m_IsAttenuated)
	{
		getAudioSource()->setPosition(m_TransformComponent->getAbsoluteTransform().Translation());
	}
}

bool AudioComponent::isLooping()
{
	return m_IsLooping;
}

void AudioComponent::setPlaying(bool enabled)
{
	if (enabled)
	{
		getAudioSource()->play();
	}
	else
	{
		getAudioSource()->stop();
	}
}

void AudioComponent::setLooping(bool enabled)
{
	m_IsLooping = enabled;
	m_AudioSource->setLooping(enabled);
}

void AudioComponent::draw()
{
	RenderSystem::GetSingleton()->submitSphere(m_TransformComponent->getAbsoluteTransform().Translation(), m_MaxDistance);

	ImGui::Checkbox("Play On Start", &m_IsPlayOnStart);
	if (ImGui::Checkbox("Looping", &m_IsLooping))
	{
		setLooping(m_IsLooping);
	}

	ImGui::Checkbox("Attenuation", &m_IsAttenuated);
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

	ImGui::DragFloat("Reference Distance", &m_ReferenceDistance, 1.0f, 0.0f, 100.0f);
	ImGui::DragFloat("Rolloff Factor", &m_RolloffFactor, 1.0f, 0, 100.0f);
	ImGui::DragFloat("Max Distance", &m_MaxDistance, 1.0f, 0, 100.0f);
}
