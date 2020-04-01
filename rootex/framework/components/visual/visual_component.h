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

enum RenderPass
{
	RenderPassMain = 1 << 0,
	RenderPassSky = 1 << 1,
	RenderPassEditor = 1 << 2,
	RenderPassUI = 1 << 3,
	RenderPassEnd
};

class VisualComponentAttributes
{
	friend class VisualComponent;
	friend class DiffuseVisualComponent;
	friend class Visual2DComponent;

protected:
	TransformComponent* m_TransformComponent;
	unsigned int m_RenderPassSetting;
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
	const unsigned int& getRenderPass() const { return m_RenderPassSetting; }
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

#ifdef ROOTEX_EDITOR
	String m_RenderPassName;
#endif // ROOTEX_EDITOR

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::VisualComponent;
	
	VisualComponent(const unsigned int& renderPassSetting, Ref<Material> material, VisualModelResourceFile* resFile, bool visibility);
	VisualComponent(VisualComponent&) = delete;
	virtual ~VisualComponent();

	virtual VisualComponentAttributes* getAttributes() { return &m_Attributes; }

	virtual bool setup() override;

	virtual bool preRender();
	virtual bool isVisible() const;
	virtual void render();
	virtual void renderChildren(const unsigned int& renderPass);
	virtual void postRender();

	void addTransform(const Matrix& applyTransform);

	void setTransform(const Matrix& newTransform);
	void setVisualModel(VisualModelResourceFile* newModel);
	void setMaterial(Ref<Material> material);
	void setPosition(const Vector3& position);
	void setVisibility(bool enabled) { m_IsVisible = enabled; }

	Vector3 getPosition() const;
	virtual String getName() const override { return "VisualComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
