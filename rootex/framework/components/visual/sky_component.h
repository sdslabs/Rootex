#pragma once

#include "component.h"
#include "core/resource_files/model_resource_file.h"
#include "renderer/materials/sky_material.h"

class SkyComponent : public Component
{
	static Component* Create(const JSON::json& componentData);

	friend class ECSFactory;

	ModelResourceFile* m_SkySphere;
	Ref<SkyMaterial> m_SkyMaterial;

	SkyComponent(const String& skyMaterialPath, const String& skySpherePath);
	SkyComponent(const SkyComponent&) = delete;
	~SkyComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::SkyComponent;

	ModelResourceFile* getSkySphere() const { return m_SkySphere; }
	SkyMaterial* getSkyMaterial() const { return m_SkyMaterial.get(); }

	virtual const char* getName() const override { return "SkyComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;

	void draw() override;
};
