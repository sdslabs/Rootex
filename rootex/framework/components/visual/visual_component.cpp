#include "visual_component.h"

#include "common/common.h"

#include "visual_component_graph.h"

#include "core/resource_loader.h"

#include "framework/entity.h"

Component* VisualComponent::Create(const LuaVariable& componentData)
{
	VisualComponent* visualComponent = new VisualComponent(
	    RenderPass::Global,
	    Ref<Material>(new Material()),
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
	bool status = true;
	for (auto& child : m_Owner->getComponent<HierarchyComponent>()->m_Children)
	{
		status = status & child->getComponent<VisualComponent>()->load(graph);
	}
	return status;
}

bool VisualComponent::preRender(VisualComponentGraph* graph)
{
	if (m_Attributes.m_TransformComponent)
	{
		graph->pushMatrix(m_Attributes.getTransform());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::ConstantBufferType::Model, graph->getTopMatrix());
	}
	else
	{
		graph->pushMatrix(Matrix::Identity);
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::ConstantBufferType::Model, graph->getTopMatrix());
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
	if (isVisible(graph))
	{
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::ConstantBufferType::View, graph->getCamera()->getView());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::ConstantBufferType::Projection, graph->getCamera()->getProjection());
	}

	for (auto& child : m_Owner->getComponent<HierarchyComponent>()->m_Children)
	{
		//TODO-FIX THIS
		VisualComponent* childVisualComponent = child->getComponent<VisualComponent>();

		childVisualComponent->preRender(graph);

		if (childVisualComponent->isVisible(graph))
		{
			// Assumed to be opaque
			childVisualComponent->render(graph);
		}
		childVisualComponent->renderChildren(graph);

		childVisualComponent->postRender(graph);
	}
}

void VisualComponent::postRender(VisualComponentGraph* graph)
{
	graph->popMatrix();
}
/*
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
*/
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
    , m_Material(Ref<Material>(new Material()))
{
}
