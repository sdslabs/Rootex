#pragma once

#include "renderer/material.h"
#include "core/resource_files/image_cube_resource_file.h"

class Texture;

class SkyMaterial : public Material
{
	SkyShader* m_SkyShader;
	ImageCubeResourceFile* m_SkyImage;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

	void setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer);

#ifdef ROOTEX_EDITOR
	String m_ImagePathUI;
#endif // ROOTEX_EDITOR
public:
	const static inline String s_MaterialName = "SkyMaterial";
	enum class VertexConstantBufferType
	{
		Model,
		End
	};
	enum class PixelConstantBufferType
	{
		End
	};

	SkyMaterial() = delete;
	SkyMaterial(const String& imagePath);
	~SkyMaterial() = default;

	void setSkyTexture(ImageCubeResourceFile* skyImageFile);

	virtual ID3D11ShaderResourceView* getPreview() override;

	static Material* CreateDefault();
	static Material* Create(const JSON::json& materialData);

	void bind() override;
	JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
