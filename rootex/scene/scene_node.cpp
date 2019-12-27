#include "scene_node.h"

#include "scene/scene.h"

SceneNode::SceneNode(const EntityID& entityID, const String& name, const AlignedMatrix& transform, AlignedMatrix* inverse, const RenderPass& renderPassSetting, const Material& material)
    : m_IsVisible(true)
{
	m_Attributes.m_EntityID = entityID;
	m_Attributes.m_Name = name;
	setTransforms(transform, inverse);
	m_Attributes.m_RenderPassSetting = renderPassSetting;
	m_Attributes.m_Material = material;
}

SceneNode::~SceneNode()
{
}

void SceneNode::setTransforms(const AlignedMatrix& transform, AlignedMatrix* inverse)
{
	m_Attributes.m_Transform = transform;
	
	if (inverse)
	{
		m_Attributes.m_InverseTransform = *inverse;
	}
	else
	{
		m_Attributes.m_InverseTransform = DirectX::XMMatrixInverse(nullptr, transform);
	}
}

bool SceneNode::load(Scene* scene)
{
	for (auto& child : m_Children)
	{
		child->load(scene);
	}
	return true;
}

bool SceneNode::preRender(Scene* scene)
{
	scene->pushMatrix(m_Attributes.getTransform());
	m_Attributes.getShader()->setConstantBuffer(Shader::ConstantBufferType::Model, *scene->getTopMatrix());
	return true;
}

bool SceneNode::isVisible(Scene* scene) const
{
	// TODO: Add culling
	WARN("Add culling code");
	return m_IsVisible;
}

void SceneNode::update(Scene* scene, float deltaMilliseconds)
{
	for (auto& child : m_Children)
	{
		child->update(scene, deltaMilliseconds);
	}
}

void SceneNode::render(Scene* scene)
{
	scene->getRenderer()->draw(m_Attributes.m_VertexBuffer, m_Attributes.m_IndexBuffer, m_Attributes.m_Shader);
}

void SceneNode::renderChildren(Scene* scene)
{
	for (auto& child : m_Children)
	{
		child->preRender(scene);

		if (child->isVisible(scene))
		{
			// Assumed to be opaque
			child->render(scene);
		}
		child->renderChildren(scene);

		child->postRender(scene);
	}
}

void SceneNode::postRender(Scene* scene)
{
	scene->popMatrix();
}

bool SceneNode::addChild(Ref<SceneNode> child)
{
	m_Children.push_back(child);
	child->m_Parent = this;
	return true;
}

bool SceneNode::removeChild(Ref<SceneNode> node)
{
	auto& findIt = std::find(m_Children.begin(), m_Children.end(), node);
	if (findIt != m_Children.end())
	{
		(*findIt)->m_Parent = nullptr;
		m_Children.erase(findIt);
	}
	return true;
}

void SceneNode::addTransform(const AlignedMatrix& applyTransform)
{
	setTransforms(m_Attributes.m_Transform * applyTransform, nullptr);
}

void SceneNode::setTransform(const AlignedMatrix& newTransform)
{
	setTransforms(newTransform, nullptr);
}

void SceneNode::setMaterial(const Material& material)
{
	m_Attributes.m_Material = material;
}

void SceneNode::setPosition(const AlignedVector& position)
{
	m_Attributes.m_Transform = DirectX::XMMatrixTranslationFromVector(position);
}

DirectX::XMFLOAT3 SceneNode::getPosition() const
{
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMStoreFloat4x4(&transform, m_Attributes.m_Transform);

	return {
		transform.m[0][3], transform.m[1][3], transform.m[2][3]
	};
}

SceneNodeAttributes::SceneNodeAttributes()
    : m_Shader(ShaderLibrary::GetShader("Default"))
{
}
