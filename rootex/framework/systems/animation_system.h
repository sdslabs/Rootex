#pragma once

#include "system.h"

class AnimationSystem : public System
{
	AnimationSystem();
	AnimationSystem(AnimationSystem&) = delete;
	~AnimationSystem() = default;

public:
	static AnimationSystem* GetSingleton();

	void update(float deltaMilliseconds) override;
};
