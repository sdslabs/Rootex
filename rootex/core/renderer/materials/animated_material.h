#pragma once

#include "renderer/material.h"

class AnimationMaterial : public Material
{
public:
	const static String s_MaterialName;
	enum class VertexConstantBufferType
	{
		Model,
		Animation,
		End
	};
	enum class PixelConstantBufferType
	{
		Color,
		End
	};

	void setVSConstantBuffer(const VSSolidConstantBuffer& constantBuffer);
	void setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer);

	AnimationMaterial();
	~AnimationMaterial() = default;

	static Material* Create(const JSON::json& materialData);
	static Material* CreateDefault();

	void bind() override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
