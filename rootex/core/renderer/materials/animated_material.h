#pragma once

#include "renderer/material.h"
#include "basic_material.h"

class AnimatedMaterial : public BasicMaterial
{
	AnimationShader* m_AnimationShader;

public:
	const static inline String s_MaterialName = "AnimatedMaterial";
	enum class VertexConstantBufferType
	{
		Model,
		Animation,
		End
	};

	AnimatedMaterial() = delete;
	AnimatedMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, const String& specularImagePath, const String& lightmapPath, bool isNormal, Color color, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky);
	~AnimatedMaterial() = default;

	void setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer);

	static Material* CreateDefault();
	static Material* Create(const JSON::json& materialData);

	void bind() override;
};
