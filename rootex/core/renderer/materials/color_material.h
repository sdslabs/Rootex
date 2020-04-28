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

	Color m_Color;

	void setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer);
	void setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer);

	void bind() override;
#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
