#include "animation_system.h"

#include "framework/components/visual/animated_character_component.h"

AnimationSystem* AnimationSystem::GetSingleton()
{
	static AnimationSystem singleton;
	return &singleton;
}

void AnimationSystem::update(float deltaMilliseconds)
{
	AnimatedCharacterComponent* animated = nullptr;
	TransformComponent* transform = nullptr;

	for (auto& component : s_Components[AnimatedCharacterComponent::s_ID])
	{
		animated = (AnimatedCharacterComponent*)component;
		transform = animated->getOwner()->getComponent<TransformComponent>().get();

		animated->m_CurrentTimePosition += deltaMilliseconds * MS_TO_S;

		if (animated->m_CurrentTimePosition >= animated->m_BoneAnimation.getEndTime())
		{
			animated->m_CurrentTimePosition = 0.0f;
		}

		animated->m_BoneAnimation.interpolate(transform->m_TransformBuffer.m_Transform, animated->m_CurrentTimePosition);
	}
}
