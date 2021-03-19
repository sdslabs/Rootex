#pragma once

#include "component.h"
#include "core/resource_files/model_resource_file.h"
#include "renderer/materials/sky_material.h"

class SkyComponent : public Component
{
	DEFINE_COMPONENT(SkyComponent);

	Ref<ModelResourceFile> m_SkySphere;
	Ref<SkyMaterial> m_SkyMaterial;

public:
	SkyComponent(Ref<ModelResourceFile> skySphere, Ref<SkyMaterial> skyMaterial);
	~SkyComponent() = default;

	ModelResourceFile* getSkySphere() const { return m_SkySphere.get(); }
	SkyMaterial* getSkyMaterial() const { return m_SkyMaterial.get(); }

	JSON::json getJSON() const override;
	void draw() override;
};
