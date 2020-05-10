#pragma once

#include "renderer/material.h"

class Texture;

class TextureMaterial : public Material
{
	TextureShader* m_DiffuseShader;
	Ref<Texture> m_DiffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

	ImageResourceFile* m_ImageFile;

	float m_SpecularIntensity;
	float m_SpecularPower;

	void setTexture(ImageResourceFile* image);

	void setPSConstantBuffer(const PSDiffuseConstantBufferLights& constantBuffer);
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
		Lights,
		Material,
		End
	};

	TextureMaterial() = delete;
	TextureMaterial(const String& imagePath);
	~TextureMaterial() = default;

	static Material* CreateDefault();
	static Material* Create(const JSON::json& materialData);

	void bind() override;
	JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
