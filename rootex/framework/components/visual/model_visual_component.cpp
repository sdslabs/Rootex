#include "model_visual_component.h"

#include "visual_component.h"

#include "common/common.h"

#include "core/resource_loader.h"
#include "event_manager.h"
#include "framework/entity.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"

Component* ModelVisualComponent::Create(const JSON::json& componentData)
{
	ModelVisualComponent* modelVisualComponent = new ModelVisualComponent(
	    componentData["renderPass"],
	    Ref<Material>(new Material()),
	    ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"]));

	modelVisualComponent->setColor(
	    Color(
	        (float)componentData["color"]["r"],
	        (float)componentData["color"]["g"],
	        (float)componentData["color"]["b"],
	        (float)componentData["color"]["a"]));

	return modelVisualComponent;
}

Component* ModelVisualComponent::CreateDefault()
{
	ModelVisualComponent* modelVisualComponent = new ModelVisualComponent(
	    RenderPassMain,
	    Ref<Material>(new Material()),
	    ResourceLoader::CreateVisualModelResourceFile("rootex/assets/cube.obj"));
	modelVisualComponent->setColor(Color(0.5f, 0.5f, 0.5f));

	return modelVisualComponent;
}

ModelVisualComponent::ModelVisualComponent(const unsigned int& renderPassSetting, Ref<Material> material,
    VisualModelResourceFile* resFile)
    : VisualComponent(renderPassSetting, isVisible)
	m_RenderPassSetting(RenderPassMain)
    , m_Material(Ref<Material>(new Material()))
    , m_VisualModelResourceFile(nullptr)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
{
	m_Attributes.m_TransformComponent = nullptr;
	m_Attributes.m_RenderPassSetting = renderPassSetting;
	m_Attributes.m_Material = material;
	m_Attributes.m_VisualModelResourceFile = resFile;
}

VisualComponent::VisualComponent(const unsigned int& renderPassSetting, const bool& isVisible)
: m_RenderPass(renderPassSetting)
    , m_IsVisible(isVisible)
{
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
	}

	return status;
}

bool ModelVisualComponent::preRender()
{
	if (m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushMatrix(m_TransformComponent->getLocalTransform());
		m_TransformComponent->m_TransformBuffer.m_AbsoluteTransform = RenderSystem::GetSingleton()->getTopMatrix();
		m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrix(Matrix::Identity);
		m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Model, RenderSystem::GetSingleton()->getTopMatrix());
	}
	PSSolidConstantBuffer Cb = { m_Color };
	m_Material->setShaderConstantBuffer(Cb);

	return true;
}

bool VisualComponent::isVisible() const
{
	// TODO: Add culling
	return m_IsVisible;
}

void VisualComponent::render()
{
	RenderSystem::GetSingleton()->getRenderer()->draw(m_Attributes.getVertexBuffer(), m_Attributes.getIndexBuffer(), m_Attributes.getMaterial());
}

void VisualComponent::renderChildren(const unsigned int& renderPass)
{
	if (isVisible() && !(renderPass & RenderPassUI))
	{
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::View, RenderSystem::GetSingleton()->getCamera()->getView());
		m_Attributes.m_Material->setShaderConstantBuffer(Shader::VertexConstantBufferType::Projection, RenderSystem::GetSingleton()->getCamera()->getProjection());
	}

	for (auto& child : m_Owner->getComponent<HierarchyComponent>()->getChildren())
	{
		VisualComponent* childVisualComponent = child->getOwner()->getComponent<VisualComponent>().get();

		if (childVisualComponent && (childVisualComponent->getAttributes()->getRenderPass() & renderPass))
		{
			childVisualComponent->preRender();

			if (childVisualComponent->isVisible())
			{
				// Assumed to be opaque
				childVisualComponent->render();
			}
			childVisualComponent->renderChildren(renderPass);

			childVisualComponent->postRender();
		}
	}
}

void VisualComponent::postRender()
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

void VisualComponent::setVisualModel(VisualModelResourceFile* newModel)
{
	m_Attributes.m_VisualModelResourceFile = newModel;
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

JSON::json VisualComponent::getJSON() const
{
	JSON::json j;

	j["resFile"] = m_Attributes.m_VisualModelResourceFile->getPath().string();
	j["renderPass"] = m_Attributes.m_RenderPassSetting;

	j["color"]["r"] = m_Color.x;
	j["color"]["g"] = m_Color.y;
	j["color"]["b"] = m_Color.z;
	j["color"]["a"] = m_Color.w;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ModelVisualComponent::draw()
{
	ImGui::BeginGroup();
	if (ImGui::BeginCombo("##Visual Model", m_VisualModelResourceFile->getPath().filename().string().c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Obj))
		{
			if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
			{
				setVisualModel((VisualModelResourceFile*)file);
			}
		}

		ImGui::Separator();

		static String inputPath = "Path";
		ImGui::InputText("##Path", &inputPath);
		ImGui::SameLine();
		if (ImGui::Button("Create Visual Model"))
		{
			if (!ResourceLoader::CreateVisualModelResourceFile(inputPath))
			{
				WARN("Could not create Visual Model");
			}
			else
			{
				inputPath = "";
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if (ImGui::Button("Visual Model"))
	{
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_VisualModelResourceFile->getPath());
	}
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (payloadPath.extension() == ".obj")
			{
				setVisualModel(ResourceLoader::CreateVisualModelResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Cannot assign a non-obj file to Visual Model");
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::ColorEdit4("Color", &m_Color.x);
}
#endif // ROOTEX_EDITOR
