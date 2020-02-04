#pragma once

#include "system.h"
#include "renderer/constant_buffer.h"
#include "components/visual/point_light_component.h"
#include "components/transform_component.h"


class PointLightSystem : public System
{
public:
	static PointLightSystem* GetSingleton();

	void update(float deltaMilliseconds) override;
	void apply();
	Lights getPointLights();
};
