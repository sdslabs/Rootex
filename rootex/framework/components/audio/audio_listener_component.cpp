#include "audio_listener_component.h"
#include "audio_bus.h"
#include "entity.h"
#include "systems/audio_system.h"

DEFINE_COMPONENT(AudioListenerComponent);

AudioListenerComponent::AudioListenerComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_Volume(data.value("volume", 1.0f))
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

Vector3 AudioListenerComponent::getPosition()
{
	return getTransformComponent()->getAbsoluteTransform().Translation();
}

Vector3 AudioListenerComponent::getUp()
{
	return getTransformComponent()->getAbsoluteTransform().Up();
}

Vector3 AudioListenerComponent::getAt()
{
	return getTransformComponent()->getAbsoluteTransform().Forward();
}

RigidBodyComponent* AudioListenerComponent::getCollider()
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

void AudioListenerComponent::draw()
{
	//AudioBusCollection obj;
	//JSON::json j;
	//obj.from_json(j,obj);
	//obj.draw();
	//obj.to_json(j,obj);
	ImGui::DragFloat("Volume", &m_Volume, 0.01f, 0.001f, FLT_MAX);
}
