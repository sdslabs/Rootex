#include "animation_system.h"

#include "framework/ecs_factory.h"
#include "components/visual/animated_model_component.h"

AnimationSystem* AnimationSystem::GetSingleton()
{
	static AnimationSystem singleton;
	return &singleton;
}

AnimationSystem::AnimationSystem()
    : System("AnimationSystem", UpdateOrder::Editor, true)
{
}

void AnimationSystem::update(float deltaMilliseconds)
{
	for (auto& component : ECSFactory::GetComponents<AnimatedModelComponent>())
	{
		AnimatedModelComponent* amc = (AnimatedModelComponent*)component;
		String animationName = amc->getCurrentAnimationName();
		if (amc->m_IsPlaying)
		{
			amc->m_CurrentTimePosition += deltaMilliseconds * MS_TO_S;
		}

		if (amc->m_CurrentTimePosition > amc->m_AnimatedModelResourceFile->getAnimationEndTime(animationName))
		{
			amc->m_CurrentTimePosition = 0.0f;
		}
		amc->m_AnimatedModelResourceFile->getFinalTransforms(animationName, amc->m_CurrentTimePosition, amc->m_FinalTransforms);
	}
}
