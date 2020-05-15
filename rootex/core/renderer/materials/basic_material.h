#pragma once

#include "renderer/material.h"

class Texture;

class BasicMaterial : public Material
{
	TextureShader* m_Shader;
	Ref<Texture> m_DiffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

	ImageResourceFile* m_ImageFile;

	bool m_IsLit = true;
	Color m_Color;
	float m_SpecularIntensity;
	float m_SpecularPower;

	void setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer);
	void setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer);

#ifdef ROOTEX_EDITOR
	String m_ImagePathUI;
#endif // ROOTEX_EDITOR
public:
	const static String s_MaterialName;
	enum class VertexConstantBufferType
	{
		Model,
		End
	};
	enum class PixelConstantBufferType
	{
		Material,
		End
	};

	BasicMaterial() = delete;
	BasicMaterial(const String& imagePath, Color color, bool isLit, float specularIntensity, float specularPower);
	~BasicMaterial() = default;

	void setColor(const Color& color) { m_Color = color; };
	void setTexture(ImageResourceFile* image);
	void setTextureInternal(Ref<Texture> texture);
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
