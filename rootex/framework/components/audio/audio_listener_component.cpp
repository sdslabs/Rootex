#include "audio_listener_component.h"

#include "entity.h"
#include "systems/audio_system.h"

Ptr<Component> AudioListenerComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<AudioListenerComponent>(
	    componentData.value("volume", 1.0f));
}

AudioListenerComponent::AudioListenerComponent(float volume)
    : m_Volume(volume)
    , m_DependencyOnTransformComponent(this)
    , m_DependencyOnBoxColliderComponent(this)
    , m_DependencyOnCapsuleColliderComponent(this)
    , m_DependencyOnSphereColliderComponent(this)
{
}

JSON::json AudioListenerComponent::getJSON() const
{
	JSON::json j;

	j["volume"] = m_Volume;

	return j;
}

void AudioListenerComponent::onRemove()
{
	if (AudioSystem::GetSingleton()->getListener() == this)
	{
		AudioSystem::GetSingleton()->restoreListener();
	}
}

void AudioListenerComponent::update()
{
	const Vector3& listenerPosition = getPosition();
	AL_CHECK(alListener3f(AL_POSITION, listenerPosition.x, listenerPosition.y, listenerPosition.z));

	const Vector3& listenerUp = getUp();
	const Vector3& listenerAt = getAt();
	float orientation[6] = {
		listenerAt.x,
		listenerAt.y,
		listenerAt.z,
		listenerUp.x,
		listenerUp.y,
		listenerUp.z,
	};
	AL_CHECK(alListenerfv(AL_ORIENTATION, orientation));

	if (RigidBodyComponent* physicsBody = getCollider())
	{
		Vector3 velocity = physicsBody->getVelocity();
		AL_CHECK(alListenerfv(AL_VELOCITY, &velocity.x));
	}
}

Vector3 AudioListenerComponent::getPosition() const
{
	return m_TransformComponent->getAbsoluteTransform().Translation();
}

Vector3 AudioListenerComponent::getUp() const
{
	return m_TransformComponent->getAbsoluteTransform().Up();
}

Vector3 AudioListenerComponent::getAt() const
{
	return m_TransformComponent->getAbsoluteTransform().Forward();
}

RigidBodyComponent* AudioListenerComponent::getCollider() const
{
	if (m_BoxColliderComponent)
	{
		return m_BoxColliderComponent;
	}
	if (m_SphereColliderComponent)
	{
		return m_SphereColliderComponent;
	}
	if (m_CapsuleColliderComponent)
	{
		return m_CapsuleColliderComponent;
	}
	return nullptr;
}

void AudioListenerComponent::draw()
{
	ImGui::DragFloat("Volume", &m_Volume, 0.01f, 0.001f, FLT_MAX);
}
