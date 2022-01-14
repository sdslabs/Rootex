#include "animation_system.h"

#include "framework/ecs_factory.h"
#include "components/visual/model/animated_model_component.h"

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
	if (!(m_IsSystemPaused))
	{
		for (auto& amc : ECSFactory::GetAllAnimatedModelComponent())
		{
			if (amc.isPlaying() && !amc.hasEnded())
			{
				amc.update(deltaMilliseconds);
			}
		}
	}
	else
	{
		for (auto& amc : ECSFactory::GetAllAnimatedModelComponent())
		{
			if (!(amc.getOwner().getScene()->getIsScenePaused()))
			{
				if (amc.isPlaying() && !amc.hasEnded())
				{
					amc.update(deltaMilliseconds);
				}
			}
		}
	}
}
