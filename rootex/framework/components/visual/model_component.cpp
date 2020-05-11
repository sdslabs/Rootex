#include "model_component.h"

#include "common/common.h"

#include "core/resource_loader.h"
#include "event_manager.h"
#include "framework/entity.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/material_library.h"

Component* ModelComponent::Create(const JSON::json& componentData)
{
	ModelComponent* modelComponent = new ModelComponent(
	    MaterialLibrary::GetMaterial(componentData["material"]),
	    ResourceLoader::CreateVisualModelResourceFile(componentData["resFile"]),
	    componentData["isVisible"]);

	return modelComponent;
}

Component* ModelComponent::CreateDefault()
{
	ModelComponent* modelComponent = new ModelComponent(
	    MaterialLibrary::GetDefaultMaterial(),
	    ResourceLoader::CreateVisualModelResourceFile("rootex/assets/cube.obj"),
	    true);

	return modelComponent;
}

ModelComponent::ModelComponent(Ref<Material> material, ModelResourceFile* resFile, bool visibility)
    : m_IsVisible(visibility)
    , m_Material(material)
    , m_VisualModelResourceFile(resFile)
    , m_HierarchyComponent(nullptr)
{
}

ModelComponent::~ModelComponent()
{
}

bool ModelComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			WARN("Entity without transform component found");
			status = false;
		}

		m_HierarchyComponent = m_Owner->getComponent<HierarchyComponent>().get();
		if (m_HierarchyComponent == nullptr)
		{
			WARN("Entity without hierarchy component found");
			status = false;
		}
	}

	return status;
}

bool ModelComponent::preRender()
{
	if (m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushMatrix(m_TransformComponent->getLocalTransform());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrix(Matrix::Identity);
	}
	return true;
}

bool ModelComponent::isVisible() const
{
	// TODO: Add culling
	return m_IsVisible;
}

void ModelComponent::render()
{
	RenderSystem::GetSingleton()->getRenderer()->draw(getVertexBuffer(), getIndexBuffer(), getMaterial());
}

void ModelComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

void ModelComponent::setVisualModel(ModelResourceFile* newModel)
{
	m_VisualModelResourceFile = newModel;
}

void ModelComponent::setMaterial(Ref<Material>& material)
{
	m_Material = material;
}

JSON::json ModelComponent::getJSON() const
{
	JSON::json j;

	j["resFile"] = m_VisualModelResourceFile->getPath().string();
	j["material"] = m_Material->getFileName();
	j["isVisible"] = m_IsVisible;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ModelComponent::draw()
{
	ImGui::BeginGroup();
	if (ImGui::BeginCombo("##Visual Model", m_VisualModelResourceFile->getPath().filename().string().c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Obj))
		{
			if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
			{
				setVisualModel((ModelResourceFile*)file);
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

	if (ImGui::BeginCombo("Material", m_Material->getFullName().c_str()))
	{
		for (auto& [materialName, materialInfo] : MaterialLibrary::GetAllMaterials())
		{
			if (m_AllowedMaterials.empty() || std::find(m_AllowedMaterials.begin(), m_AllowedMaterials.end(), materialInfo.first) != m_AllowedMaterials.end())
			{
				if (ImGui::Selectable((materialName + " - " + materialInfo.first).c_str()))
				{
					setMaterial(MaterialLibrary::GetMaterial(String(materialName)));
				}
			}
		}
		ImGui::EndCombo();
	}

	m_Material->draw();
}
#endif // ROOTEX_EDITOR
