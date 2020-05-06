#pragma once

#include "core/renderer/material_library.h"
#include "visual_component.h"

class ModelVisualComponent : public VisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

protected:
	VisualModelResourceFile* m_VisualModelResourceFile;
	Ref<Material> m_Material; //change
	HierarchyComponent* m_HierarchyComponent;

	ModelVisualComponent(const unsigned int& renderPassSetting, Ref<Material> material, VisualModelResourceFile* resFile, bool isVisible);
	ModelVisualComponent(ModelVisualComponent&) = delete;
	virtual ~ModelVisualComponent();

#ifdef ROOTEX_EDITOR
	/// Empty Vector means all materials are allowed
	Vector<String> m_AllowedMaterials;
#endif // ROOTEX_EDITOR
public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ModelVisualComponent;

	virtual bool setup() override;

	virtual bool preRender() override;
	virtual bool isVisible() const override;
	virtual void render(RenderPass renderPass) override;
	virtual void renderChildren(RenderPass renderPass) override;
	virtual void postRender() override;

	void setVisualModel(VisualModelResourceFile* newModel);
	void setMaterial(Ref<Material>& material);//change
	
	const VertexBuffer* getVertexBuffer() const { return m_VisualModelResourceFile->getVertexBuffer(); }
	const IndexBuffer* getIndexBuffer() const { return m_VisualModelResourceFile->getIndexBuffer(); }
	Material* getMaterial() { return m_Material.get(); }//change
	VisualModelResourceFile* getModelResourceFile() const { return m_VisualModelResourceFile; }

	virtual String getName() const override { return "ModelVisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
