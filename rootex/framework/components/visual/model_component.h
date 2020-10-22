#pragma once

#include "component.h"
#include "components/hierarchy_component.h"
#include "components/transform_component.h"
#include "renderer/material.h"
#include "renderer/mesh.h"
#include "core/resource_files/model_resource_file.h"

class ModelComponent : public Component
{
	DEPENDS_ON(TransformComponent);
	DEPENDS_ON(HierarchyComponent);

	static Component* Create(const JSON::json& componentData);

	friend class EntityFactory;

protected:
	ModelResourceFile* m_ModelResourceFile;
	bool m_IsVisible;
	int m_RenderPass;

	HashMap<Ref<Material>, Ref<Material>> m_MaterialOverrides;
	Vector<EntityID> m_AffectingStaticLightEntityIDs;
	Vector<int> m_AffectingStaticLights;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_PerModelCB;

	ModelComponent(unsigned int renderPass, ModelResourceFile* resFile, const HashMap<String, String>& materialOverrides, bool isVisible, const Vector<EntityID>& affectingStaticLightIDs);
	ModelComponent(ModelComponent&) = delete;
	virtual ~ModelComponent() = default;

	void assignBoundingBox();
	void assignOverrides(ModelResourceFile* newModel, const HashMap<String, String>& materialOverrides);

#ifdef ROOTEX_EDITOR
	/// Empty Vector means all materials are allowed
	Vector<String> m_AllowedMaterials;
#endif // ROOTEX_EDITOR
public:
	static void RegisterAPI(sol::table& rootex);
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ModelComponent;

	virtual bool setupData() override;
	virtual bool setupEntities() override;

	virtual bool preRender(float deltaMilliseconds);
	virtual bool isVisible() const;
	virtual void render();
	virtual void postRender();

	bool addAffectingStaticLight(EntityID ID);
	void removeAffectingStaticLight(EntityID ID);
	
	void setVisualModel(ModelResourceFile* newModel, const HashMap<String, String>& materialOverrides);
	void setIsVisible(bool enabled);
	void setMaterialOverride(Ref<Material> oldMaterial, Ref<Material> newMaterial);
	
	unsigned int getRenderPass() const { return m_RenderPass; }
	const Vector<Pair<Ref<Material>, Vector<Mesh>>>& getMeshes() const { return m_ModelResourceFile->getMeshes(); }
	ModelResourceFile* getModelResourceFile() const { return m_ModelResourceFile; }

	virtual const char* getName() const override { return "ModelComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
