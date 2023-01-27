#pragma once

#include "common/common.h"

#include "renderer/shader.h"
#include "material_resource_file.h"

/// Representation of a custom material.
class CustomMaterialResourceFile : public MaterialResourceFile
{
private:
	static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> s_Sampler;
	Vector<float> customConstantBuffers;
	Vector<TYPES_OF_BUFFERS> typeOfCustomConstantBuffers;

	CustomMaterialData m_MaterialData;

	String m_DummyVertexShaderPath;
	int m_PixelShaderCount;
	int index=0;
	std::vector<Ptr<Shader>> m_Shader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSCB;
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

	void setShaders(int ind,const String& vertexShader, const String& pixelShader);
	void setVS(int ind,const String& vertexShader);
	void setPS(int ind,const String& pixelShader);
	void recompileShaders(int ind);

	const Shader* getShader() const override
	{
		if (!(m_Shader.empty()))
		{
			return m_Shader[0].get();
		}
		else
			return nullptr;
	}; // doubt??
	Vector<Ref<GPUTexture>> getTextures() const override;

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

	float getFloat(int index);
	Vector3 getFloat3(int index);
	Color getColor(int index);

	bool setFloat(int index, float value);
	bool setFloat3(int index, Vector3 value);
	bool setColor(int index, Color value);
};
