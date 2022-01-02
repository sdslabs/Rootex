#pragma once

#include "resource_file.h"
#include "renderer/shader.h"
#include "basic_material_resource_file.h"
#include "image_resource_file.h"
#include "core/renderer/vertex_data.h"

/// Representation of a hardware instancing material.
class InstancingBasicMaterialResourceFile : public BasicMaterialResourceFile
{
private:
	static inline Ptr<Shader> s_Shader;

public:
	static void Load();
	static void Destroy();

	explicit InstancingBasicMaterialResourceFile(const FilePath& path);
	~InstancingBasicMaterialResourceFile() = default;

	const Shader* getShader() const override { return s_Shader.get(); };

	void bindShader() override;
	std::string getClassName()
	{
		return std::string("InstancingBasicMaterialResourceFile");
	}
};
