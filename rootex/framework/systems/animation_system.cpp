#include "animation_system.h"

#include "components/visual/animated_model_component.h"

AnimationSystem* AnimationSystem::GetSingleton()
{
	static AnimationSystem singleton;
	return &singleton;
}

AnimationSystem::AnimationSystem()
    : System("AnimationSystem", UpdateOrder::Update, true)
{
}

void AnimationSystem::update(float deltaMilliseconds)
{
	AnimatedModelComponent* animationComponent = nullptr;
	for (auto& component : s_Components[AnimatedModelComponent::s_ID])
	{
		animationComponent = dynamic_cast<AnimatedModelComponent*>(component);

		if (animationComponent)
		{
			String animationName = animationComponent->getCurrentAnimationName();

			if (animationComponent->m_IsPlaying)
			{
				animationComponent->m_CurrentTimePosition += deltaMilliseconds * MS_TO_S;
			}

			if (animationComponent->m_CurrentTimePosition > animationComponent->m_AnimatedModelResourceFile->getAnimationEndTime(animationName))
			{
				animationComponent->m_CurrentTimePosition = 0.0f;
			}

			animationComponent->m_AnimatedModelResourceFile->getFinalTransforms(animationName, animationComponent->m_CurrentTimePosition, animationComponent->m_FinalTransforms);
		}
	}
}
