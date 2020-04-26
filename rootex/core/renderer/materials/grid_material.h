#pragma once

#include "renderer/material.h"

class GridMaterial : public Material
{
public:
	GridMaterial();
	~GridMaterial() = default;

	void bind() override;
};
