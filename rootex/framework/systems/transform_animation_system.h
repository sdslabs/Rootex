#pragma once

#include "system.h"

class TransformAnimationSystem : public System
{
public:
	static TransformAnimationSystem* GetSingleton();

	TransformAnimationSystem();

	void begin();
	void update(float deltaMilliseconds) override;
};
