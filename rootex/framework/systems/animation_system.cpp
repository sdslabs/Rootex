#include "animation_system.h"

#include "components/animation_component.h"
#include "components/transform_animation_component.h"
#include "components/transform_component.h"

AnimationSystem* AnimationSystem::GetSingleton()
{
	static AnimationSystem singleton;
	return &singleton;
}

void AnimationSystem::update(float deltaMilliseconds)
{
	TransformAnimationComponent* animated = nullptr;
	TransformComponent* transform = nullptr;

	for (auto& component : s_Components[TransformAnimationComponent::s_ID])
	{
		animated = (TransformAnimationComponent*)component;
		if (animated->isPlaying())
		{
			transform = animated->getOwner()->getComponent<TransformComponent>().get();

			animated->m_CurrentTimePosition += deltaMilliseconds * MS_TO_S;

			if (animated->m_CurrentTimePosition >= animated->m_BasicAnimation.getEndTime())
			{
				if (animated->isLooping())
				{
					animated->m_CurrentTimePosition = 0.0f;
				}
			}

			animated->m_BasicAnimation.interpolate(transform->m_TransformBuffer.m_Transform, animated->m_CurrentTimePosition);
		}
	}

	AnimationComponent* skeletalAnimation;
	for (auto& component : s_Components[AnimationComponent::s_ID])
	{
		skeletalAnimation = (AnimationComponent*)component;
		skeletalAnimation->m_CurrentTimePosition += deltaMilliseconds * MS_TO_S;

		skeletalAnimation->m_AnimationFile->getFinalTransforms("Root|Walk_loop", deltaMilliseconds * MS_TO_S, skeletalAnimation->m_FinalTransforms);
	}
}
