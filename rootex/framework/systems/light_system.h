#pragma once

#include "system.h"
#include "renderer/constant_buffer.h"

/// Interface for setting up point, directional and spot lights.
class LightSystem : public System
{
	LightSystem();

public:
	static LightSystem* GetSingleton();

	StaticPointLightsInfo getStaticPointLights();
	LightsInfo getDynamicLights();
};
