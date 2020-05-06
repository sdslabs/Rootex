#pragma once

#include "renderer/material.h"

class ColorMaterial : public Material
{
	void setPSConstantBuffer(const PSSolidConstantBuffer& constantBuffer);
	void setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer);
	Color m_Color;

public:
	const static String s_MaterialName;
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

	Color getColor() { return m_Color; };
	void setColor(const Color& color) { m_Color = color; };

	static Material* CreateDefault();
	static Material* Create(const JSON::json& materialData);

	void bind() override;
	JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
