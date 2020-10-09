#include "transform_animation_system.h"

#include "components/transform_animation_component.h"

TransformAnimationSystem* TransformAnimationSystem::GetSingleton()
{
	static TransformAnimationSystem singleton;
	return &singleton;
}

TransformAnimationSystem::TransformAnimationSystem()
    : System("TransformationAnimationSystem", UpdateOrder::Update, true)
{
}

void TransformAnimationSystem::begin()
{
	TransformAnimationComponent* animation = nullptr;
	for (auto& component : s_Components[TransformAnimationComponent::s_ID])
	{
		animation = (TransformAnimationComponent*)component;
		
		if (animation->isPlayOnStart())
		{
			animation->setPlaying(true);
		}
	}
}

void TransformAnimationSystem::update(float deltaMilliseconds)
{
	TransformAnimationComponent* animation = nullptr;
	for (auto& component : s_Components[TransformAnimationComponent::s_ID])
	{
		animation = (TransformAnimationComponent*)component;

		if (animation->isPlaying() && !animation->hasEnded())
		{
			animation->interpolate(deltaMilliseconds * MS_TO_S);
		}
	}
}
