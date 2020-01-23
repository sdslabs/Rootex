#pragma once

#include "system.h"

class PointLightSystem : public System
{
public:
	void update(float deltaMilliseconds) override;
	void apply();
	static PointLightSystem* GetSingleton();
};
