#pragma once

#include "component.h"
#include "core/renderer/renderer.h"

#include "renderer/index_buffer.h"
#include "renderer/material.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_buffer.h"

#include "components/transform_component.h"
#include "components/hierarchy_component.h"

class HierarchyGraph;

enum class RenderPass
{
	Global,
	Background,
	Static,
	Dynamic,
	Editor,
	End
};

class VisualComponentAttributes
{
	friend class VisualComponent;
	friend class DiffuseVisualComponent;

protected:
	TransformComponent* m_TransformComponent;
	RenderPass m_RenderPassSetting;
	VisualModelResourceFile* m_VisualModelResourceFile;
	Ref<Material> m_Material;
	HierarchyComponent* m_HierarchyComponent;

public:
	VisualComponentAttributes();
	~VisualComponentAttributes() = default;

	const VertexBuffer* getVertexBuffer() const { return m_VisualModelResourceFile->getVertexBuffer(); }
	const IndexBuffer* getIndexBuffer() const { return m_VisualModelResourceFile->getIndexBuffer(); }
	const Matrix& getTransform() const { return m_TransformComponent->getLocalTransform(); }
	const Matrix& getInverseTransform() const { return m_TransformComponent->getLocalTransform().Invert(); }
	const RenderPass& getRenderPass() const { return m_RenderPassSetting; }
	Material* getMaterial() { return m_Material.get(); }
	VisualModelResourceFile* getModelResourceFile() const { return m_VisualModelResourceFile; }
};

class VisualComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Color m_Color;
	void setColor(const Color& color) { m_Color = color; };

protected:
	VisualComponentAttributes m_Attributes;
	bool m_IsVisible;
	
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::VisualComponent;
	
	VisualComponent(const RenderPass& renderPassSetting, Ref<Material> material, VisualModelResourceFile* resFile);
	VisualComponent(VisualComponent&) = delete;
	virtual ~VisualComponent();

	virtual VisualComponentAttributes* getAttributes() { return &m_Attributes; }

	bool setup() override;

	virtual bool preRender(HierarchyGraph* graph);
	virtual bool isVisible(HierarchyGraph* graph) const;
	virtual void render(HierarchyGraph* graph);
	virtual void renderChildren(HierarchyGraph* graph);
	virtual void postRender(HierarchyGraph* graph);

	void addTransform(const Matrix& applyTransform);

	void setTransform(const Matrix& newTransform);
	void setMaterial(Ref<Material> material);
	void setPosition(const Vector3& position);
	void setVisibility(bool enabled) { m_IsVisible = enabled; }

	Vector3 getPosition() const;

	virtual String getName() const override { return "VisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;
};
