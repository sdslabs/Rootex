#include "visual_component.h"

#include "visual_component_graph.h"

#include "core/resource_loader.h"

#include "framework/entity.h"

Component* VisualComponent::Create(const LuaVariable& componentData)
{
	VisualComponent* visualComponent = new VisualComponent(
	    RenderPass::Global,
	    Material::CreateDefault(),
	    ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"].tostring()));

	return visualComponent;
}

VisualComponent::VisualComponent(const RenderPass& renderPassSetting, Ref<Material> material, VisualModelResourceFile* resFile)
    : m_IsVisible(true)
{
	m_Attributes.m_TransformComponent = nullptr;
	m_Attributes.m_RenderPassSetting = renderPassSetting;
	m_Attributes.m_Material = material;
	m_Attributes.m_VisualModelResourceFile = resFile;
	m_Parent = nullptr;
}

VisualComponent::~VisualComponent()
{
}

bool VisualComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_Attributes.m_TransformComponent = m_Owner->getComponent<TransformComponent>();
		if (m_Attributes.m_TransformComponent == nullptr)
		{
			status = false;
		}
	}

	return status;
}

bool VisualComponent::load(VisualComponentGraph* graph)
{
	for (auto& child : m_Children)
	{
		child->load(graph);
	}
	return true;
}

bool VisualComponent::preRender(VisualComponentGraph* graph)
{
	if (m_Attributes.m_TransformComponent)
	{
		graph->pushMatrix(m_Attributes.getTransform());
		m_Attributes.m_Material->setShaderMatrix(Shader::ConstantBufferType::Model, graph->getTopMatrix());
	}
	else
	{
		graph->pushMatrix(Matrix::Identity);
		m_Attributes.m_Material->setShaderMatrix(Shader::ConstantBufferType::Model, graph->getTopMatrix());
	}
	return true;
}

bool VisualComponent::isVisible(VisualComponentGraph* graph) const
{
	// TODO: Add culling
	return m_IsVisible;
}

void VisualComponent::render(VisualComponentGraph* graph)
{
	graph->getRenderer()->draw(m_Attributes.getVertexBuffer(), m_Attributes.getIndexBuffer(), m_Attributes.getMaterial());
}

void VisualComponent::renderChildren(VisualComponentGraph* graph)
{
	for (auto& child : m_Children)
	{
		child->preRender(graph);

		if (child->isVisible(graph))
		{
			// Assumed to be opaque
			child->render(graph);
		}
		child->renderChildren(graph);

		child->postRender(graph);
	}
}

void VisualComponent::postRender(VisualComponentGraph* graph)
{
	graph->popMatrix();
}

bool VisualComponent::addChild(VisualComponent* child)
{
	m_Children.push_back(Ref<VisualComponent>(child));
	child->m_Parent = this;
	return true;
}

bool VisualComponent::removeChild(Ref<VisualComponent> node)
{
	auto& findIt = std::find(m_Children.begin(), m_Children.end(), node);
	if (findIt != m_Children.end())
	{
		(*findIt)->m_Parent = nullptr;
		m_Children.erase(findIt);
	}
	return true;
}

void VisualComponent::addTransform(const Matrix& applyTransform)
{
	m_Attributes.m_TransformComponent->setTransform(m_Attributes.m_TransformComponent->getTransform() * applyTransform);
}

void VisualComponent::setTransform(const Matrix& newTransform)
{
	m_Attributes.m_TransformComponent->setTransform(newTransform);
}

void VisualComponent::setMaterial(Ref<Material> material)
{
	m_Attributes.m_Material = material;
}

void VisualComponent::setPosition(const Vector3& position)
{
	m_Attributes.m_TransformComponent->setPosition(position);
}

Vector3 VisualComponent::getPosition() const
{
	return m_Attributes.m_TransformComponent->getPosition();
}

VisualComponentAttributes::VisualComponentAttributes()
    : m_RenderPassSetting(RenderPass::Global)
    , m_Material(Material::CreateDefault())
{
}
