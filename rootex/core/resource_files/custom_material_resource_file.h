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

	void pushPSTexture(Ref<ImageResourceFile> texture);
	void setPSTexture(const String& newtexturePath, int position);
	void popPSTexture();

	void pushVSTexture(Ref<ImageResourceFile> texture);
	void setVSTexture(const String& newtexturePath, int position);
	void popVSTexture();

public:
	static inline const String s_DefaultCustomVSPath = "rootex/core/renderer/shaders/custom_vertex_shader.hlsl";
	static inline const String s_DefaultCustomPSPath = "rootex/core/renderer/shaders/custom_pixel_shader.hlsl";

	static void Load();
	static void Destroy();

	explicit CustomMaterialResourceFile(const FilePath& path);
	~CustomMaterialResourceFile() = default;

	void setShaders(const String& vertexShader, const String& pixelShader);
	void setVS(const String& vertexShader);
	void setPS(const String& pixelShader);
	void recompileShaders();

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
	void drawTextureSlots(const char* label, Vector<Ref<ImageResourceFile>>& textures);
};
