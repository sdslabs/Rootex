#pragma once

#include "resource_file.h"
#include "renderer/shader.h"
#include "material_resource_file.h"
#include "image_resource_file.h"

/// Representation of a Basic material.
class BasicMaterialResourceFile : public MaterialResourceFile
{
private:
	static inline Ptr<Shader> s_Shader;
	static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> s_Sampler;

	BasicMaterialData m_MaterialData;

	Ref<ImageResourceFile> m_DiffuseImageFile;
	Ref<ImageResourceFile> m_NormalImageFile;
	Ref<ImageResourceFile> m_SpecularImageFile;
	Ref<ImageResourceFile> m_LightmapImageFile;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSCB;

	BasicMaterialResourceFile(const Type type, const FilePath& path);

public:
	static void Load();
	static void Destroy();

	explicit BasicMaterialResourceFile(const FilePath& path);
	virtual ~BasicMaterialResourceFile() = default;

	void setColor(const Color& color);
	void setDiffuse(Ref<ImageResourceFile> diffuse);
	void setNormal(Ref<ImageResourceFile> normal);
	void setSpecular(Ref<ImageResourceFile> spec);
	void setLightmap(Ref<ImageResourceFile> lightmap);

	const Shader* getShader() const override { return s_Shader.get(); };
	void bindShader() override;
	void bindTextures() override;
	void bindSamplers() override;
	void bindVSCB() override;
	void bindPSCB() override;

	Ref<ImageResourceFile> getDiffuseImageFile() { return m_DiffuseImageFile; }

	JSON::json getJSON() const override;
	ID3D11ShaderResourceView* getPreview() const override;

	void reimport() override;
	bool save() override;
	void draw() override;
};
