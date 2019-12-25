#pragma once

#include "common/common.h"
#include "framework/entity.h"
#include "renderer/material.h"
#include "renderer/vertex_buffer.h"
#include "renderer/index_buffer.h"
#include "renderer/shader_library.h"

enum class RenderPass
{
	Global,
	Background,
	Static,
	Dynamic,
	Editor,
	End
};

class SceneNodeAttributes
{
	friend class SceneNode;

protected:
	EntityID m_EntityID;
	String m_Name;
	AlignedMatrix m_Transform;
	AlignedMatrix m_InverseTransform;
	//float m_Radius; Needed only while raycasting
	RenderPass m_RenderPassSetting;
	Material m_Material;
	VertexBuffer m_VertexBuffer;
	IndexBuffer m_IndexBuffer;
	Shader* m_Shader;

public:
	SceneNodeAttributes();
	~SceneNodeAttributes() = default;

	const EntityID& getEntityID() const { return m_EntityID; }
	const String& getName() const { return m_Name; }
	const VertexBuffer& getVertexBuffer() const { return m_VertexBuffer; }
	const IndexBuffer& getIndexBuffer() const { return m_IndexBuffer; }
	const AlignedMatrix& getTransform() const { return m_Transform; }
	const AlignedMatrix& getInverseTransform() const { return m_InverseTransform; }
	const RenderPass& getRenderPass() const { return m_RenderPassSetting; }
	Material& getMaterial() { return m_Material; }
	Shader* getShader() const { return m_Shader; }
};

class Scene;

class SceneNode
{
protected:
	SceneNode* m_Parent;
	Vector<Ref<SceneNode>> m_Children;
	SceneNodeAttributes m_Attributes;

public:
	SceneNode(const EntityID& entityID,
	    const String& name,
	    AlignedMatrix* transform,
	    AlignedMatrix* inverse,
	    //float m_Radius; Needed only while raycasting
	    const RenderPass& renderPassSetting,
	    const Material& material);
	SceneNode(SceneNode&) = delete;
	~SceneNode();

	virtual const SceneNodeAttributes* getAttributes() const { return &m_Attributes; }
	virtual void setTransforms(AlignedMatrix* transform, AlignedMatrix* inverse);
	
	virtual bool load(Scene* scene);
	
	virtual bool preRender(Scene* scene);
	virtual bool isVisible(Scene* scene) const;
	virtual void update(Scene* scene, float deltaMilliseconds);
	virtual void render(Scene* scene);
	virtual void renderChildren(Scene* scene);
	virtual void postRender(Scene* scene);

	virtual bool addChild(Ref<SceneNode> child);
	virtual bool removeChild(Ref<SceneNode> node);

	void setMaterial(const Material& material);
	void setPosition(const AlignedVector& position);
	
	DirectX::XMFLOAT3 getPosition() const;
};
