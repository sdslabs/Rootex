#pragma once

#include "framework/scene.h"
#include "framework/system.h"

class TransformSystem : public System
{
	TransformSystem();
	TransformSystem(TransformSystem&) = delete;

	Vector<Matrix> m_TransformationStack;

public:
	static TransformSystem* GetSingleton();

	void calculateTransforms(Scene* scene);

	void pushMatrix(const Matrix& transform);
	void pushMatrixOverride(const Matrix& transform);
	void popMatrix();
	const Matrix& getCurrentMatrix() const;
};
