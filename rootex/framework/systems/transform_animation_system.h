#pragma once

#include "system.h"

/// System which handles the initialisation and updation of transform animations.
class TransformAnimationSystem : public System
{
public:
	static TransformAnimationSystem* GetSingleton();

	TransformAnimationSystem();

	void begin();
	void update(float deltaMilliseconds) override;
};
