#include "audio_listener_component.h"

#include "entity.h"
#include "components/space/transform_component.h"
#include "systems/audio_system.h"

Ptr<Component> AudioListenerComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<AudioListenerComponent>();
}

AudioListenerComponent::AudioListenerComponent()
    : m_DependencyOnTransformComponent(this)
{
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
