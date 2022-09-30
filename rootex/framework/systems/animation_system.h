#pragma once

#include "system.h"

/// System that handles all skeletal animations.
class AnimationSystem : public System
{
	AnimationSystem();
	AnimationSystem(AnimationSystem&) = delete;

public:
	static AnimationSystem* GetSingleton();

	void update(float deltaMilliseconds) override;
};
