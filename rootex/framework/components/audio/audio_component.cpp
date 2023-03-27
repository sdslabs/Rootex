#include "audio_component.h"

#include "al.h"
#include "systems/render_system.h"
#include "systems/audio_system.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/physics/capsule_collider_component.h"

AudioComponent::AudioComponent(
    Entity& owner,
    bool playOnStart,
    float volume,
    bool isLooping,
    bool attenuation,
    AudioSource::AttenuationModel model,
    ALfloat rolloffFactor,
    ALfloat referenceDistance,
    ALfloat maxDistance)
    : Component(owner)
    , m_IsPlayOnStart(playOnStart)
    , m_Volume(volume)
    , m_IsAttenuated(attenuation)
    , m_IsLooping(isLooping)
    , m_AttenuationModel(model)
    , m_RolloffFactor(rolloffFactor)
    , m_ReferenceDistance(referenceDistance)
    , m_MaxDistance(maxDistance)
    , m_DependencyOnTransformComponent(this)
    , m_DependencyOnBoxColliderComponent(this)
    , m_DependencyOnCapsuleColliderComponent(this)
    , m_DependencyOnSphereColliderComponent(this)
{
}

RigidBodyComponent* AudioComponent::getCollider()
{
	if (getBoxColliderComponent())
	{
		return getBoxColliderComponent();
	}
	if (getSphereColliderComponent())
	{
		return getSphereColliderComponent();
	}
	if (getCapsuleColliderComponent())
	{
		return getCapsuleColliderComponent();
	}
	return nullptr;
}

bool AudioComponent::setupData()
{
	if (m_IsAttenuated)
	{
		getAudioSource()->setPosition(getTransformComponent()->getParentAbsoluteTransform().Translation());
		getAudioSource()->setModel(m_AttenuationModel);
		getAudioSource()->setRollOffFactor(m_RolloffFactor);
		getAudioSource()->setReferenceDistance(m_ReferenceDistance);
		getAudioSource()->setMaxDistance(m_MaxDistance);
		getAudioSource()->setVolume(m_Volume);
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
	j["volume"] = m_Volume;

	return j;
}

void AudioComponent::update()
{
	if (m_IsAttenuated)
	{
		getAudioSource()->setPosition(getTransformComponent()->getAbsoluteTransform().Translation());
	}

	if (RigidBodyComponent* physicsBody = getCollider())
	{
		getAudioSource()->setVelocity(physicsBody->getVelocity());
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

void AudioComponent::play()
{
	setPlaying(true);
}

void AudioComponent::stop()
{
	setPlaying(false);
}

void AudioComponent::setLooping(bool enabled)
{
	m_IsLooping = enabled;
	m_AudioSource->setLooping(enabled);
}

void AudioComponent::draw()
{
	RenderSystem::GetSingleton()->submitSphere(getTransformComponent()->getAbsoluteTransform().Translation(), m_MaxDistance);

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
	ImGui::DragFloat("Rolloff Factor", &m_RolloffFactor, 1.0f, 0.0f, 100.0f);
	ImGui::DragFloat("Max Distance", &m_MaxDistance, 1.0f, 0.0f, 100.0f);
	if(ImGui::DragFloat("Volume", &m_Volume, 0.01f, 0.0f, 100.0f))
	{
		AL_CHECK(alSourcef(1, AL_GAIN, m_Volume));
	}
}
