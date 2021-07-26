#pragma once

#include "core/renderer/renderer.h"
#include "core/renderer/render_pass.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "main/window.h"
#include "framework/ecs_factory.h"
#include "framework/scene.h"
#include "framework/system.h"
#include "framework/components/visual/camera_component.h"
#include "components/visual/model/model_component.h"
#include "components/visual/model/animated_model_component.h"
#include "components/game/spring_arm_component.h"

#include "ASSAO/ASSAO.h"

class TransformSystem : public System
{

	TransformSystem();
	TransformSystem(TransformSystem&) = delete;

public:
	static TransformSystem* GetSingleton();

	void calculateTransforms(Scene* scene);
};
