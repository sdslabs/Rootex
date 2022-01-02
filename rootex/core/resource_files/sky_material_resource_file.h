#pragma once

#include "resource_file.h"
#include "renderer/shader.h"
#include "material_resource_file.h"
#include "image_cube_resource_file.h"

/// Representation of a sky material.
class SkyMaterialResourceFile : public MaterialResourceFile
{
private:
	static inline Ptr<Shader> s_Shader;
	static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> s_Sampler;

	SkyMaterialData m_MaterialData;

	Ref<ImageCubeResourceFile> m_SkyFile;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSCB;

public:
	static void Load();
	static void Destroy();

	explicit SkyMaterialResourceFile(const FilePath& path);
	~SkyMaterialResourceFile() = default;

	void setSky(Ref<ImageCubeResourceFile> sky);

	const Shader* getShader() const override { return s_Shader.get(); };
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
	std::string getClassName()
	{
		return std::string("SkyMaterialResourceFile");
	}
};
