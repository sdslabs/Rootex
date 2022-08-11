#pragma once

#include "framework/system.h"
#include "framework/components/space/transform_component.h"

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
