#pragma once

#include "system.h"

/// Render system for the UI.
class RenderUISystem : public System
{
	Vector<Matrix> m_UITransformationStack;

	RenderUISystem();
	RenderUISystem(RenderUISystem&) = delete;

public:
	static RenderUISystem* GetSingleton();

	void update(float deltaMilliseconds);

	void pushUIMatrix(const Matrix& transform);
	void popUIMatrix();

	Matrix getTopUIMatrix();
};
