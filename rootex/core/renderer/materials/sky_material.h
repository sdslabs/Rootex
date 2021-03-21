#pragma once

#include "renderer/material.h"
#include "core/resource_files/image_cube_resource_file.h"

class Texture;

class SkyMaterial : public Material
{
	SkyShader* m_SkyShader;
	Ref<ImageCubeResourceFile> m_SkyImage;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;

	void setVSConstantBuffer(const VSDiffuseConstantBuffer& constantBuffer);

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

	void setSkyTexture(Ref<ImageCubeResourceFile> skyImageFile);

	virtual ID3D11ShaderResourceView* getPreview() override;

	static Material* CreateDefault();
	static Material* Create(const JSON::json& materialData);

	void bind() override;
	JSON::json getJSON() const override;

	void draw() override;
};
