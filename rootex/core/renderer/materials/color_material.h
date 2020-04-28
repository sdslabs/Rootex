#pragma once

#include "renderer/material.h"

class ColorMaterial : public Material
{
public:
	enum class VertexConstantBufferType
	{
		Model,
		End
	};
	enum class PixelConstantBufferType
	{
		Color,
		End
	};

	ColorMaterial();
	~ColorMaterial() = default;

	void setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer);
	void setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer);

	void bind() override;
};
