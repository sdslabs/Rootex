#include "visual_component.h"

#include "common/common.h"

#include "core/resource_loader.h"

#include "framework/entity.h"
#include "framework/systems/render_system.h"
#include "framework/systems/light_system.h"

Component* VisualComponent::Create(const JSON::json& componentData)
{
	VisualComponent* visualComponent = new VisualComponent(
	    RenderPass::Global,
	    Ref<Material>(new Material()),
	    ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"])
	);
	visualComponent->setColor(Color((float)componentData["color"]["r"], (float)componentData["color"]["g"],
	    (float)componentData["color"]["b"], (float)componentData["color"]["a"]));

	return visualComponent;
}

Component* VisualComponent::CreateDefault()
{
	VisualComponent* visualComponent = new VisualComponent(
	    RenderPass::Global,
	    Ref<Material>(new Material()),
	    ResourceLoader::CreateVisualModelResourceFile("rootex/assets/cube.obj"));
	visualComponent->setColor(Color(0.5f, 0.5f, 0.5f));

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

#ifdef ROOTEX_EDITOR
	// TODO: Remove this if statement when camera gets a resource file
	if (resFile)
	{
		m_ModelPathUI = resFile->getPath().string();
	}
#endif // ROOTEX_EDITOR
}

VisualComponent::~VisualComponent()
{
}

bool VisualComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_Attributes.m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_Attributes.m_TransformComponent == nullptr)
		{
			status = false;
		}

		m_Attributes.m_HierarchyComponent = m_Owner->getComponent<HierarchyComponent>().get();
		if (m_Attributes.m_HierarchyComponent == nullptr)
		{
			WARN("Entity without hierarchy component found");
			status = false;
		}

		HierarchySystem::GetSingleton()->getRootHierarchyComponent()->addVCToRenderPass(m_Owner->getComponent<VisualComponent>(), m_Owner);
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
		VisualComponent* childVisualComponent = child->getComponent<VisualComponent>().get();

		if (childVisualComponent)
		{
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

void VisualComponent::onRemove()
{
	if (m_Owner)
	{
		Ref<Entity> parent = m_Owner->getComponent<HierarchyComponent>()->getParent();
		if (parent->getID() == ROOT_ENTITY_ID)
		{
			Ref<RootHierarchyComponent> rootHC = std::dynamic_pointer_cast<RootHierarchyComponent>(parent->getComponent<HierarchyComponent>());
			rootHC->removeVCFromRenderPass(m_Owner->getComponent<VisualComponent>(), m_Owner);
		}
	}
}

Vector3 VisualComponent::getPosition() const
{
	return m_Attributes.m_TransformComponent->getPosition();
}

JSON::json VisualComponent::getJSON() const
{
	JSON::json j;

	j["resFile"] = m_Attributes.m_VisualModelResourceFile->getPath().string();

	j["color"]["r"] = m_Color.x;
	j["color"]["g"] = m_Color.y;
	j["color"]["b"] = m_Color.z;
	j["color"]["a"] = m_Color.w;

	return j;
}

VisualComponentAttributes::VisualComponentAttributes()
    : m_RenderPassSetting(RenderPass::Global)
    , m_Material(Ref<Material>(new Material()))
    , m_VisualModelResourceFile(nullptr)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
{
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void VisualComponent::draw()
{
	if (ImGui::InputText("Visual Model", &m_ModelPathUI, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		VisualModelResourceFile* model = ResourceLoader::CreateVisualModelResourceFile(m_ModelPathUI);
		if (model)
		{
			m_Attributes.m_VisualModelResourceFile = model;
		}
		else
		{
			m_ModelPathUI = m_Attributes.m_VisualModelResourceFile->getPath().string();
		}
	}

	ImGui::ColorEdit4("Color", &m_Color.x);
}
#endif // ROOTEX_EDITOR
