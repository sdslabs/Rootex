#pragma once

#include "system.h"

class TransformAnimationSystem : public System
{
public:
	static TransformAnimationSystem* GetSingleton();

	void update(float deltaMilliseconds);
};
