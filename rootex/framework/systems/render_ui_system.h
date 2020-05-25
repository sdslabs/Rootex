#pragma once

#include "system.h"

class RenderUISystem : public System
{
	Vector<Matrix> m_UITransformationStack;

	RenderUISystem();
	RenderUISystem(RenderUISystem&) = delete;
	~RenderUISystem() = default;

public:
	static RenderUISystem* GetSingleton();

	void render();

	void pushUIMatrix(const Matrix& transform);
	void popUIMatrix();

	Matrix getTopUIMatrix();
};
