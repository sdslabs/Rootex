#pragma once

#include "component.h"
#include "components/hierarchy_component.h"
#include "components/transform_component.h"
#include "renderer/material.h"
#include "core/resource_file.h"

class ModelComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

protected:
	ModelResourceFile* m_ModelResourceFile;
	bool m_IsVisible;
	unsigned int m_RenderPass;

	HierarchyComponent* m_HierarchyComponent;
	TransformComponent* m_TransformComponent;

	ModelComponent(unsigned int renderPass, ModelResourceFile* resFile, bool isVisible);
	ModelComponent(ModelComponent&) = delete;
	virtual ~ModelComponent() = default;

#ifdef ROOTEX_EDITOR
	/// Empty Vector means all materials are allowed
	Vector<String> m_AllowedMaterials;
#endif // ROOTEX_EDITOR
public:
	static void RegisterAPI(sol::state& rootex);
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ModelComponent;

	virtual bool setup() override;

	virtual bool preRender();
	virtual bool isVisible() const;
	virtual void render();
	virtual void postRender();

	void setVisualModel(ModelResourceFile* newModel);
	void setIsVisible(bool enabled);
	
	unsigned int getRenderPass() const { return m_RenderPass; }
	const HashMap<Ref<Material>, Vector<Mesh>>& getMeshes() const { return m_ModelResourceFile->getMeshes(); }
	ModelResourceFile* getModelResourceFile() const { return m_ModelResourceFile; }

	virtual String getName() const override { return "ModelComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
