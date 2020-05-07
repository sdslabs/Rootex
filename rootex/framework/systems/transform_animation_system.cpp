#include "transform_animation_system.h"

#include "components/transform_animation_component.h"

TransformAnimationSystem* TransformAnimationSystem::GetSingleton()
{
	static TransformAnimationSystem singleton;
	return &singleton;
}

void TransformAnimationSystem::update(float deltaMilliseconds)
{
	TransformAnimationComponent* animation = nullptr;
	for (auto& component : s_Components[TransformAnimationComponent::s_ID])
	{
		animation = (TransformAnimationComponent*)component;

		if (animation->isPlaying())
		{
			animation->m_CurrentTimePosition += deltaMilliseconds * MS_TO_S;

			if (animation->hasEnded() && animation->isLooping())
			{
				animation->m_CurrentTimePosition = 0.0f;
			}

			animation->interpolate(animation->m_CurrentTimePosition);
		}
	}
}
