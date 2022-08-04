#pragma once

#include "core/renderer/renderer.h"
#include "core/renderer/render_pass.h"
#include "core/resource_files/basic_material_resource_file.h"
#include "main/window.h"
#include "framework/ecs_factory.h"
#include "framework/scene.h"
#include "framework/system.h"
#include "render_system.h"

class TransformSystem : public System
{

	Vector<Matrix> m_TransformationStack;

	TransformSystem();
	TransformSystem(TransformSystem&) = delete;

public:
	static TransformSystem* GetSingleton();

	void calculateTransforms(Scene* scene);
	void pushMatrix(const Matrix& transform);
	void pushMatrixOverride(const Matrix& transform);
	void popMatrix();

	const Matrix& getCurrentMatrix() const;
};
