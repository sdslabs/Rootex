#pragma once

#include "component.h"

#include "renderer/materials/sky_material.h"

class SkyComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

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

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
