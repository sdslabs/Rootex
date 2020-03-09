#include "visual_component.h"

#include "common/common.h"

#include "core/resource_loader.h"

#include "framework/entity.h"
#include "framework/systems/render_system.h"
#include "framework/systems/light_system.h"

Component* VisualComponent::Create(const LuaVariable& componentData)
{
	VisualComponent* visualComponent = new VisualComponent(
	    RenderPass::Global,
	    Ref<Material>(new Material()),
	    ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"].tostring())
	);
	visualComponent->setColor(Color((float)componentData["color"]["r"], (float)componentData["color"]["g"],
	    (float)componentData["color"]["b"], (float)componentData["color"]["a"]));

	return visualComponent;
}

VisualComponent::VisualComponent(const RenderPass& renderPassSetting, Ref<Material> material,
	VisualModelResourceFile* resFile)
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

		m_Attributes.m_HierarchyComponent = m_Owner->getComponent<HierarchyComponent>();
		if (m_Attributes.m_HierarchyComponent == nullptr)
		{
			WARN("Entity without hierarchy component found");
			status = false;
		}
	}

	return status;
}

bool VisualComponent::preRender(HierarchyGraph* graph)
{
	if (m_Attributes.m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushMatrix(m_Attributes.getTransform());
		m_Attributes.m_TransformComponent->m_TransformBuffer.m_AbsoluteTransform = RenderSystem::GetSingleton()->getTopMatrix();
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrix(Matrix::Identity);
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
	}
	PSSolidConstantBuffer Cb = { m_Color };
	m_Attributes.m_Material->setShaderConstantBuffer(Cb);
	
	return true;
}

bool VisualComponent::isVisible(HierarchyGraph* graph) const
{
	// TODO: Add culling
	return m_IsVisible;
}

void VisualComponent::render(HierarchyGraph* graph)
{
	RenderSystem::GetSingleton()->getRenderer()->draw(m_Attributes.getVertexBuffer(), m_Attributes.getIndexBuffer(), m_Attributes.getMaterial());
}

void VisualComponent::renderChildren(HierarchyGraph* graph)
{
	if (isVisible(graph))
	{
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::View, RenderSystem::GetSingleton()->getCamera()->getView());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Projection, RenderSystem::GetSingleton()->getCamera()->getProjection());
	}

	for (auto& child : m_Owner->getComponent<HierarchyComponent>()->getChildren())
	{
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

void VisualComponent::postRender(HierarchyGraph* graph)
{
	RenderSystem::GetSingleton()->popMatrix();
}

void VisualComponent::addTransform(const Matrix& applyTransform)
{
	m_Attributes.m_TransformComponent->setTransform(m_Attributes.m_TransformComponent->getLocalTransform() * applyTransform);
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
    , m_VisualModelResourceFile(nullptr)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
{
}
