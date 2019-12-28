#include "scene_node.h"

#include "scene/scene.h"

SceneNode::SceneNode(const EntityID& entityID, const String& name, const Matrix& transform, Matrix* inverse, const RenderPass& renderPassSetting, const Material& material, RenderableObject* object)
    : m_IsVisible(true)
{
	m_Attributes.m_EntityID = entityID;
	m_Attributes.m_Name = name;
	setTransforms(transform, inverse);
	m_Attributes.m_RenderPassSetting = renderPassSetting;
	m_Attributes.m_Material = material;
	m_Attributes.m_RenderableObject.reset(object);
}

SceneNode::SceneNode(const EntityID& entityID, const String& name, const Matrix& transform, Matrix* inverse, const RenderPass& renderPassSetting, const Material& material)
    : m_IsVisible(true)
{
	m_Attributes.m_EntityID = entityID;
	m_Attributes.m_Name = name;
	setTransforms(transform, inverse);
	m_Attributes.m_RenderPassSetting = renderPassSetting;
	m_Attributes.m_Material = material;
	m_Attributes.m_RenderableObject.reset(new Empty());
}

SceneNode::~SceneNode()
{
}

void SceneNode::setTransforms(const Matrix& transform, Matrix* inverse)
{
	m_Attributes.m_Transform = transform;
	
	if (inverse)
	{
		m_Attributes.m_InverseTransform = *inverse;
	}
	else
	{
		m_Attributes.m_InverseTransform = transform.Invert();
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
	//m_Attributes.getShader()->setConstantBuffer(Shader::ConstantBufferType::Model, *scene->getTopMatrix());
	m_Attributes.getRenderableObject()->Update(*scene->getTopMatrix());
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
	//scene->getRenderer()->draw(m_Attributes.m_VertexBuffer, m_Attributes.m_IndexBuffer, m_Attributes.m_Shader);
	m_Attributes.getRenderableObject()->Draw();
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

void SceneNode::addTransform(const Matrix& applyTransform)
{
	setTransforms(m_Attributes.m_Transform * applyTransform, nullptr);
}

void SceneNode::setTransform(const Matrix& newTransform)
{
	setTransforms(newTransform, nullptr);
}

void SceneNode::setMaterial(const Material& material)
{
	m_Attributes.m_Material = material;
}

void SceneNode::setPosition(const Vector3& position)
{
	m_Attributes.m_Transform = Matrix::CreateTranslation(position);
}

Vector3 SceneNode::getPosition() const
{
	return m_Attributes.m_Transform.Translation();
}

SceneNodeAttributes::SceneNodeAttributes()
    : m_Shader(ShaderLibrary::GetShader("Default"))
{
}
