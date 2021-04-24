#pragma once

#include "common/common.h"

#include "renderer/shader.h"
#include "material_resource_file.h"

/// Representation of a custom material.
class CustomMaterialResourceFile : public MaterialResourceFile
{
private:
	static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> s_Sampler;

	CustomMaterialData m_MaterialData;

	Ptr<Shader> m_Shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSCB;

	void pushTexture(Ref<ImageResourceFile> texture);
	void setTexture(const String& newtexturePath, int position);
	void popTexture();

public:
	static inline const String s_DefaultCustomShaderPath = "rootex/core/renderer/shaders/custom_pixel_shader.hlsl";

	static void Load();
	static void Destroy();

	explicit CustomMaterialResourceFile(const FilePath& path);
	~CustomMaterialResourceFile() = default;

	void setPixelShader(const String& pixelShader);

	const Shader* getShader() const override { return m_Shader.get(); };
	void bindShader() override;
	void bindTextures() override;
	void bindSamplers() override;
	void bindVSCB() override;
	void bindPSCB() override;

	JSON::json getJSON() const override;
	ID3D11ShaderResourceView* getPreview() const override;

	void reimport() override;
	bool save() override;
	void draw() override;
};
