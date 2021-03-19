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
	for (auto& component : ECSFactory::GetComponents<AnimatedModelComponent>())
	{
		AnimatedModelComponent* amc = (AnimatedModelComponent*)component;
		if (amc->isPlaying() && !amc->hasEnded())
		{
			amc->update(deltaMilliseconds);
		}
	}
}
