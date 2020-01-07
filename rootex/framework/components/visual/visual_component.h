#pragma once

#include "component.h"
#include "core/renderer/renderer.h"

#include "renderer/index_buffer.h"
#include "renderer/material.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/vertex_buffer.h"

#include "components/transform_component.h"

class VisualComponentGraph;

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

protected:
	TransformComponent* m_TransformComponent;
	RenderPass m_RenderPassSetting;
	VisualModelResourceFile* m_VisualModelResourceFile;
	Ref<Material> m_Material;
	Ref<Texture> m_Texture;

public:
	VisualComponentAttributes();
	~VisualComponentAttributes() = default;

	const VertexBuffer* getVertexBuffer() const { return m_VisualModelResourceFile->getVertexBuffer(); }
	const IndexBuffer* getIndexBuffer() const { return m_VisualModelResourceFile->getIndexBuffer(); }
	const Matrix& getTransform() const { return m_TransformComponent->getTransform(); }
	const Matrix& getInverseTransform() const { return m_TransformComponent->getTransform().Invert(); }
	const RenderPass& getRenderPass() const { return m_RenderPassSetting; }
	Material* getMaterial() { return m_Material.get(); }
	Texture* getTexture() { return m_Texture.get(); }
	VisualModelResourceFile* getModelResourceFile() const { return m_VisualModelResourceFile; }
};

class VisualComponent : public Component
{
	static Component* Create(const LuaVariable& componentData);

protected:
	VisualComponent* m_Parent;
	Vector<Ref<VisualComponent>> m_Children;
	VisualComponentAttributes m_Attributes;
	bool m_IsVisible;
	
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::Visual;
	
	VisualComponent(const RenderPass& renderPassSetting, Ref<Material> material, Ref<Texture> texture, VisualModelResourceFile* resFile);
	VisualComponent(VisualComponent&) = delete;
	virtual ~VisualComponent();

	virtual VisualComponentAttributes* getAttributes() { return &m_Attributes; }

	bool setup() override;
	virtual bool load(VisualComponentGraph* visualComponentGraph);

	virtual bool preRender(VisualComponentGraph* visualComponentGraph);
	virtual bool isVisible(VisualComponentGraph* visualComponentGraph) const;
	virtual void render(VisualComponentGraph* visualComponentGraph);
	virtual void renderChildren(VisualComponentGraph* visualComponentGraph);
	virtual void postRender(VisualComponentGraph* visualComponentGraph);

	virtual bool addChild(VisualComponent* child);
	virtual bool removeChild(Ref<VisualComponent> node);
	void addTransform(const Matrix& applyTransform);

	void setTransform(const Matrix& newTransform);
	void setMaterial(Ref<Material> material);
	void setPosition(const Vector3& position);
	void setVisibility(bool enabled) { m_IsVisible = enabled; }

	Vector3 getPosition() const;

	ComponentID getComponentID() const override { return s_ID; }
};
