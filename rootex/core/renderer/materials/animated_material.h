#pragma once

#include "renderer/material.h"

class AnimationMaterial : public Material
{
public:
	const static String s_MaterialName;
	enum class VertexConstantBufferType
	{
		Animation,
		End
	};
	enum class PixelConstantBufferType
	{
		Color,
		End
	};

	Vector<Matrix> m_BoneTransforms;

	void setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer);

	AnimationMaterial();
	~AnimationMaterial() = default;

	static Material* Create(const JSON::json& materialData);
	static Material* CreateDefault();

	void bind() override;
	JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
