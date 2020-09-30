#pragma once

#include "renderer/material.h"

class AnimatedMaterial : public Material
{
	AnimationShader* m_AnimationShader;
	Ref<Texture> m_DiffuseTexture;
	Ref<Texture> m_NormalTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
	
	ImageResourceFile* m_ImageFile;
	ImageResourceFile* m_NormalImageFile;

	bool m_IsLit;
	bool m_IsNormal;
	Color m_Color;
	float m_SpecularIntensity;
	float m_SpecularPower;
	float m_Reflectivity;
	float m_RefractionConstant;
	float m_Refractivity;
	bool m_IsAffectedBySky;

	void setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantbuffer);
	void setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer);

#ifdef ROOTEX_EDITOR
	String m_ImagePathUI;
#endif // ROOTEX_EDITOR
public:
	const static inline String s_MaterialName = "AnimatedMaterial";
	enum class VertexConstantBufferType
	{
		Model,
		Animation,
		End
	};
	enum class PixelConstantBufferType
	{
		Material,
		End
	};

	AnimatedMaterial() = delete;
	AnimatedMaterial(bool isAlpha, const String& imagePath, const String& normalImagePath, bool isNormal, Color color, bool isLit, float specularIntensity, float specularPower, float reflectivity, float refractionConstant, float refractivity, bool affectedBySky);
	~AnimatedMaterial() = default;
	
	void setVSConstantBuffer(const VSAnimationConstantBuffer& constantBuffer);

	void setColor(const Color& color) { m_Color = color; };
	void setTexture(ImageResourceFile* image);
	void setNormal(ImageResourceFile* image);
	void removeNormal();
	void setTextureInternal(Ref<Texture> texture);
	void setNormalInternal(Ref<Texture> texture);
	void setSpecularIntensity(float specIntensity) { m_SpecularIntensity = specIntensity; }
	void setSpecularPower(float specPower) { m_SpecularPower = specPower; }

	static Material* CreateDefault();
	static Material* Create(const JSON::json& materialData);

	void bind() override;
	JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw(const String& id) override;
#endif // ROOTEX_EDITOR
};
