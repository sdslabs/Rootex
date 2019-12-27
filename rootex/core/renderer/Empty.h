#pragma once
#include "renderable_object.h"

class Empty : public RenderableObject
{
public:
	Empty() = default;
	void Update(const AlignedMatrix& transform) override;
	void Draw() override;
};