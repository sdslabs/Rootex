#pragma once

#include "system.h"

class AnimationSystem : public System
{
public:
	static AnimationSystem* GetSingleton();

	void update(float deltaMilliseconds);
};
