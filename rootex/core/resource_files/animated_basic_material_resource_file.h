#pragma once

#include "resource_file.h"
#include "renderer/shader.h"
#include "basic_material_resource_file.h"
#include "image_resource_file.h"

/// Representation of a skeletal animation basic material.
class AnimatedBasicMaterialResourceFile : public BasicMaterialResourceFile
{
private:
	static inline Ptr<Shader> s_Shader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_BonesVSCB;

public:
	static void Load();
	static void Destroy();

	explicit AnimatedBasicMaterialResourceFile(const FilePath& path);
	~AnimatedBasicMaterialResourceFile() = default;

	void uploadAnimationBuffer(const PerModelAnimationVSCBData& animationBuffer);

	const Shader* getShader() const override { return s_Shader.get(); };

	void bindShader() override;
	void bindVSCB() override;

	void reimport() override;
};
