#pragma once

#include "resource_file.h"
#include "renderer/shader.h"
#include "material_resource_file.h"
#include "image_resource_file.h"
/// Representation of a material used for drawing decals. Uses ImageResourceFile to store the Decal's texture.
class DecalMaterialResourceFile : public MaterialResourceFile
{
private:
	static inline Ptr<Shader> s_Shader;
	static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> s_Sampler;

	DecalMaterialData m_MaterialData;

	Ref<ImageResourceFile> m_DecalImageFile;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PSCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VSCB;

	DecalMaterialResourceFile(const Type type, const FilePath& path);

public:
	static void Load();
	static void Destroy();

	explicit DecalMaterialResourceFile(const FilePath& path);
	virtual ~DecalMaterialResourceFile() = default;

	void setColor(const Color& color);
	void setDecal(Ref<ImageResourceFile> decal);

	const Shader* getShader() const override { return s_Shader.get(); };
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
};
