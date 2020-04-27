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
	    MaterialLibrary::GetMaterial((String)componentData["material"]),
	    ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"]),
	    componentData["isVisible"]);

	return modelVisualComponent;
}

Component* ModelVisualComponent::CreateDefault()
{
	ModelVisualComponent* modelVisualComponent = new ModelVisualComponent(
	    RenderPassMain,
	    MaterialLibrary::GetDefaultMaterial(), //change
	    ResourceLoader::CreateVisualModelResourceFile("rootex/assets/cube.obj"),
	    true);

	return modelVisualComponent;
}

ModelVisualComponent::ModelVisualComponent(const unsigned int& renderPassSetting, Ref<Material> material, VisualModelResourceFile* resFile, bool visibility) //change
    : VisualComponent(renderPassSetting, visibility)
    , m_Material(material)
    , m_VisualModelResourceFile(resFile)
    , m_HierarchyComponent(nullptr)
{
}

ModelVisualComponent::~ModelVisualComponent()
{
}

bool ModelVisualComponent::setup()
{
	bool status = VisualComponent::setup();
	if (m_Owner)
	{
		m_HierarchyComponent = m_Owner->getComponent<HierarchyComponent>().get();
		if (m_HierarchyComponent == nullptr)
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
		RenderSystem::GetSingleton()->pushMatrix(getTransform());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrix(Matrix::Identity);	
	}
	return true;
}

bool ModelVisualComponent::isVisible() const
{
	// TODO: Add culling
	return VisualComponent::isVisible();
}

void ModelVisualComponent::render(RenderPass renderPass)
{
	if (renderPass & m_RenderPass)
	{
		RenderSystem::GetSingleton()->getRenderer()->draw(getVertexBuffer(), getIndexBuffer(), getMaterial());
	}
}

void ModelVisualComponent::renderChildren(RenderPass renderPass)
{
	for (auto& child : m_Owner->getComponent<HierarchyComponent>()->getChildren())
	{
		VisualComponent* childVisualComponent = child->getOwner()->getComponent<VisualComponent>().get();

		if (childVisualComponent)
		{
			childVisualComponent->preRender();

			if (childVisualComponent->isVisible())
			{
				// Assumed to be opaque
				childVisualComponent->render(renderPass);
			}
			childVisualComponent->renderChildren(renderPass);

			childVisualComponent->postRender();
		}
	}
}

void ModelVisualComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

void ModelVisualComponent::setVisualModel(VisualModelResourceFile* newModel)
{
	m_VisualModelResourceFile = newModel;
}

void ModelVisualComponent::setMaterial(Ref<Material>& material) //change
{
	m_Material = material;
}

JSON::json ModelVisualComponent::getJSON() const
{
	JSON::json& j = VisualComponent::getJSON();

	j["resFile"] = m_VisualModelResourceFile->getPath().string();
	j["material"] = m_Material->getFileName();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ModelVisualComponent::draw()
{
	VisualComponent::draw();

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
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_VisualModelResourceFile->getPath().string());
	}
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (IsSupported(payloadPath.extension().string(), SupportedModelExtensions))
			{
				setVisualModel(ResourceLoader::CreateVisualModelResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Cannot assign an unsupported model file to Visual Model");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginCombo("Material", m_Material->getFullName().c_str()))
	{
		for (auto& [materialName, materialInfo] : MaterialLibrary::GetAllMaterials())
		{
			if (ImGui::Selectable((materialInfo.first + " - " + materialName).c_str()))
			{
				setMaterial(MaterialLibrary::GetMaterial(String(materialName)));
			}
		}
		ImGui::EndCombo();
	}

	m_Material->draw();
}
#endif // ROOTEX_EDITOR
