#pragma once
#include "renderable_object.h"

class Empty : public RenderableObject
{
public:
	Empty() = default;
	void Update(const Matrix& transform) override;
	void Draw() override;
};