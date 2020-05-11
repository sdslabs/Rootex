#pragma once

#include "component.h"
#include "components/hierarchy_component.h"
#include "components/transform_component.h"
#include "renderer/material.h"

class ModelComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

protected:
	ModelResourceFile* m_VisualModelResourceFile;
	Ref<Material> m_Material;
	bool m_IsVisible;

	HierarchyComponent* m_HierarchyComponent;
	TransformComponent* m_TransformComponent;

	ModelComponent(Ref<Material> material, ModelResourceFile* resFile, bool isVisible);
	ModelComponent(ModelComponent&) = delete;
	virtual ~ModelComponent();

#ifdef ROOTEX_EDITOR
	/// Empty Vector means all materials are allowed
	Vector<String> m_AllowedMaterials;
#endif // ROOTEX_EDITOR
public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ModelComponent;

	virtual bool setup() override;

	virtual bool preRender();
	virtual bool isVisible() const;
	virtual void render();
	virtual void postRender();

	void setVisualModel(ModelResourceFile* newModel);
	void setMaterial(Ref<Material>& material);
	
	const VertexBuffer* getVertexBuffer() const { return m_VisualModelResourceFile->getVertexBuffer(); }
	const IndexBuffer* getIndexBuffer() const { return m_VisualModelResourceFile->getIndexBuffer(); }
	Material* getMaterial() { return m_Material.get(); }
	ModelResourceFile* getModelResourceFile() const { return m_VisualModelResourceFile; }

	virtual String getName() const override { return "ModelComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
