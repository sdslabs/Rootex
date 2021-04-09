#pragma once

#include "component.h"
#include "core/resource_files/model_resource_file.h"
#include "core/resource_files/sky_material_resource_file.h"

class SkyComponent : public Component
{
	DEFINE_COMPONENT(SkyComponent, Category::Effect);

	Ref<ModelResourceFile> m_SkySphere;
	Ref<SkyMaterialResourceFile> m_SkyMaterial;

public:
	SkyComponent(Entity& owner, const JSON::json& data);
	~SkyComponent() = default;

	ModelResourceFile* getSkySphere() const { return m_SkySphere.get(); }
	SkyMaterialResourceFile* getSkyMaterial() const { return m_SkyMaterial.get(); }

	JSON::json getJSON() const override;
	void draw() override;
};
