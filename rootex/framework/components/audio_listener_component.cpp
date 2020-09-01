#include "audio_listener_component.h"

#include "entity.h"
#include "transform_component.h"
#include "systems/audio_system.h"

Component* AudioListenerComponent::Create(const JSON::json& componentData)
{
	return new AudioListenerComponent();
}

Component* AudioListenerComponent::CreateDefault()
{
	return new AudioListenerComponent();
}

AudioListenerComponent::AudioListenerComponent()
    : m_TransformComponent(nullptr)
{
}

bool AudioListenerComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		if (Ref<TransformComponent> transform = m_Owner->getComponent<TransformComponent>())
		{
			m_TransformComponent = transform.get();
		}
		else
		{
			ERR("TransformComponent not found on entity with AudioListenerComponent: " + m_Owner->getFullName());
			status = false;
		}
	}
	return status;
}

void AudioListenerComponent::onRemove()
{
	if (AudioSystem::GetSingleton()->getListener() == this)
	{
		AudioSystem::GetSingleton()->restoreListener();
	}
}

Vector3 AudioListenerComponent::getPosition() const
{
	return m_TransformComponent->getAbsoluteTransform().Translation();
}
