#pragma once

#include "system.h"

class RenderUISystem : public System
{
	Vector<Matrix> m_UITransformationStack;

	RenderUISystem();
	RenderUISystem(RenderUISystem&) = delete;
	virtual ~RenderUISystem() = default;

public:
	static RenderUISystem* GetSingleton();

	void update(float deltaMilliseconds);

	void pushUIMatrix(const Matrix& transform);
	void popUIMatrix();

	Matrix getTopUIMatrix();

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
