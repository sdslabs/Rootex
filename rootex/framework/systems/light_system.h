#pragma once

#include "system.h"
#include "renderer/constant_buffer.h"
#include "components/visual/point_light_component.h"
#include "components/visual/directional_light_component.h"
#include "components/visual/spot_light_component.h"
#include "components/transform_component.h"


class LightSystem : public System
{
public:
	static LightSystem* GetSingleton();

	Lights getLights();
};
