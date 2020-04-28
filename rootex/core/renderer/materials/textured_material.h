#pragma once

#include "renderer/material.h"

class Texture;

class TexturedMaterial : public Material
{
	DiffuseShader* m_DiffuseShader;
	Ref<Texture> m_DiffuseTexture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

	ImageResourceFile* m_ImageFile;

	void setTexture(ImageResourceFile* image);

#ifdef ROOTEX_EDITOR
	String m_ImagePathUI;
#endif // ROOTEX_EDITOR
public:
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
	TexturedMaterial();
	~TexturedMaterial() = default;

	void setPSConstantBuffer(const PSDiffuseConstantBufferLights& constantBuffer);
	void setPSConstantBuffer(const PSDiffuseConstantBufferMaterial& constantBuffer);
	void setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer);

	void bind() override;
#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
