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

	ColorMaterial() = delete;
	ColorMaterial(const Color& color);
	~ColorMaterial() = default;

	Color m_Color;

	void setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer);
	void setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer);

	static Material* CreateDefault();
	static Material* Create(const JSON::json& materialData);

	void bind() override;
	JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
