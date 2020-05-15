#include "model_component.h"

#include "common/common.h"

#include "core/resource_loader.h"
#include "event_manager.h"
#include "framework/entity.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "renderer/material_library.h"
#include "renderer/render_pass.h"

Component* ModelComponent::Create(const JSON::json& componentData)
{
	ModelComponent* modelComponent = new ModelComponent(
	    componentData["renderPass"],
	    ResourceLoader::CreateModelResourceFile(componentData["resFile"]),
	    componentData["isVisible"]);

	return modelComponent;
}

Component* ModelComponent::CreateDefault()
{
	ModelComponent* modelComponent = new ModelComponent(
	    (unsigned int)RenderPass::Basic,
	    ResourceLoader::CreateModelResourceFile("rootex/assets/cube.obj"),
	    true);

	return modelComponent;
}

ModelComponent::ModelComponent(unsigned int renderPass, ModelResourceFile* resFile, bool visibility)
    : m_IsVisible(visibility)
    , m_RenderPass(renderPass)
    , m_ModelResourceFile(resFile)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
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
		RenderSystem::GetSingleton()->pushMatrixOverride(m_TransformComponent->getAbsoluteTransform());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrixOverride(Matrix::Identity);
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
	for (auto& mesh : m_ModelResourceFile->getMeshes())
	{
		RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.m_IndexBuffer.get(), mesh.m_Material.get());
	}
}

void ModelComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

void ModelComponent::setVisualModel(ModelResourceFile* newModel)
{
	m_ModelResourceFile = newModel;
}

void ModelComponent::setIsVisible(bool enabled)
{
	m_IsVisible = enabled;
}

JSON::json ModelComponent::getJSON() const
{
	JSON::json j;

	j["resFile"] = m_ModelResourceFile->getPath().string();
	j["isVisible"] = m_IsVisible;
	j["renderPass"] = m_RenderPass;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ModelComponent::draw()
{
	ImGui::BeginGroup();
	String modelFileName = m_ModelResourceFile ? m_ModelResourceFile->getPath().filename().string() : "None";
	if (ImGui::BeginCombo("##Visual Model", modelFileName.c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Model))
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
			if (!ResourceLoader::CreateModelResourceFile(inputPath))
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

	if (ImGui::Button("Model"))
	{
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_ModelResourceFile->getPath().string());
	}
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (IsFileSupported(payloadPath.extension().string(), ResourceFile::Type::Model))
			{
				setVisualModel(ResourceLoader::CreateModelResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Unsupported file format for Model");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::TreeNodeEx("Materials"))
	{
		int i = 0;
		for (auto& mesh : m_ModelResourceFile->getMeshes())
		{
			if (ImGui::BeginCombo(("Material " + std::to_string(i)).c_str(), mesh.m_Material->getFullName().c_str()))
			{
				for (auto& [materialName, materialInfo] : MaterialLibrary::GetAllMaterials())
				{
					if (m_AllowedMaterials.empty() || std::find(m_AllowedMaterials.begin(), m_AllowedMaterials.end(), materialInfo.first) != m_AllowedMaterials.end())
					{
						if (ImGui::Selectable((materialName + " - " + materialInfo.first).c_str()))
						{
							Ref<BasicMaterial> material = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial(materialName));

							if (material)
							{
								mesh.m_Material = material;
							}
							else
							{
								WARN("Invalid material type");
							}
						}
					}
				}
				ImGui::EndCombo();
			}

			mesh.m_Material->draw(std::to_string(i));
			ImGui::Separator();
			i++;
		}
		ImGui::TreePop();
	}
}
#endif // ROOTEX_EDITOR
