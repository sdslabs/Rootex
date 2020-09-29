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
	HashMap<String, String> materialOverrides;
	if (componentData.find("materialOverrides") != componentData.end())
	{
		for (auto& element : JSON::json::iterator_wrapper(componentData["materialOverrides"]))
		{
			materialOverrides[element.key()] = element.value();
		}
	}

	ModelComponent* modelComponent = new ModelComponent(
	    componentData["renderPass"],
	    ResourceLoader::CreateModelResourceFile(componentData["resFile"]),
	    materialOverrides,
	    componentData["isVisible"]);

	return modelComponent;
}

Component* ModelComponent::CreateDefault()
{
	ModelComponent* modelComponent = new ModelComponent(
	    (int)RenderPass::Basic,
	    ResourceLoader::CreateModelResourceFile("rootex/assets/cube.obj"),
	    {},
	    true);

	return modelComponent;
}

ModelComponent::ModelComponent(unsigned int renderPass, ModelResourceFile* resFile, const HashMap<String, String>& materialOverrides, bool visibility)
    : m_IsVisible(visibility)
    , m_RenderPass(renderPass)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
{
	setVisualModel(resFile, materialOverrides);
}

void ModelComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<ModelComponent> modelComponent = rootex.new_usertype<ModelComponent>(
	    "ModelComponent",
	    sol::base_classes, sol::bases<Component>());
	rootex["Entity"]["getModel"] = &Entity::getComponent<ModelComponent>;
	modelComponent["isVisible"] = &ModelComponent::isVisible;
	modelComponent["setIsVisible"] = &ModelComponent::setIsVisible;
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

bool ModelComponent::preRender(float deltaMilliseconds)
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

bool compareMaterials(const Pair<Ref<Material>, Vector<Mesh>>& a, const Pair<Ref<Material>, Vector<Mesh>>& b)
{
	// Alpha materials final last
	return !a.first->isAlpha() && b.first->isAlpha();
}

void ModelComponent::render()
{
	std::sort(m_ModelResourceFile->getMeshes().begin(), m_ModelResourceFile->getMeshes().end(), compareMaterials);
	int i = 0;
	for (auto& [material, meshes] : m_ModelResourceFile->getMeshes())
	{
		RenderSystem::GetSingleton()->getRenderer()->bind(m_MaterialOverrides[material].get());
		i++;

		for (auto& mesh : meshes)
		{
			RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.m_IndexBuffer.get());
		}
	}
}

void ModelComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

void ModelComponent::setVisualModel(ModelResourceFile* newModel, const HashMap<String, String>& materialOverrides)
{
	if (!newModel)
	{
		return;
	}

	m_ModelResourceFile = newModel;
	m_MaterialOverrides.clear();
	for (auto& [material, meshes] : m_ModelResourceFile->getMeshes())
	{
		setMaterialOverride(material, material);
	}
	for (auto& [oldMaterial, newMaterial] : materialOverrides)
	{
		MaterialLibrary::CreateNewMaterialFile(newMaterial, MaterialLibrary::GetMaterial(oldMaterial)->getTypeName());
		setMaterialOverride(MaterialLibrary::GetMaterial(oldMaterial), MaterialLibrary::GetMaterial(newMaterial));
	}
}

void ModelComponent::setIsVisible(bool enabled)
{
	m_IsVisible = enabled;
}

void ModelComponent::setMaterialOverride(Ref<Material> oldMaterial, Ref<Material> newMaterial)
{
	m_MaterialOverrides[oldMaterial] = newMaterial;
}

JSON::json ModelComponent::getJSON() const
{
	JSON::json j;

	j["resFile"] = m_ModelResourceFile->getPath().string();
	j["isVisible"] = m_IsVisible;
	j["renderPass"] = m_RenderPass;

	j["materialOverrides"] = {};
	for (auto& [oldMaterial, newMaterial] : m_MaterialOverrides)
	{
		j["materialOverrides"][oldMaterial->getFileName()] = newMaterial->getFileName();
	}

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ModelComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	ImGui::BeginGroup();

	String inputPath = m_ModelResourceFile->getPath().generic_string();
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

	ImGui::SameLine();

	if (ImGui::Button("Model"))
	{
		EventManager::GetSingleton()->call("OpenModel", "EditorOpenFile", m_ModelResourceFile->getPath().string());
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
				setVisualModel(ResourceLoader::CreateModelResourceFile(payloadPath.string()), {});
			}
			else
			{
				WARN("Unsupported file format for Model");
			}
		}
		ImGui::EndDragDropTarget();
	}

	int renderPassUI = log2(m_RenderPass);
	if (ImGui::Combo("Renderpass", &renderPassUI, "Basic\0Editor\0Alpha"))
	{
		m_RenderPass = pow(2, renderPassUI);
	}

	ImGui::Columns(2);
	ImGui::Text("%s", "Original Material");
	ImGui::NextColumn();
	ImGui::Text("%s", "Overriding Material");
	ImGui::NextColumn();
	ImGui::Separator();
	for (auto& [oldMaterial, newMaterial] : m_MaterialOverrides)
	{
		ImGui::BeginGroup();
		ImGui::Image(oldMaterial->getPreview(), { 50, 50 });
		ImGui::SameLine();
		if (ImGui::MenuItem(FilePath(oldMaterial->getFileName()).filename().generic_string().c_str()))
		{
			EventManager::GetSingleton()->call("OpenModel", "EditorOpenFile", oldMaterial->getFileName());
		}
		ImGui::NextColumn();
		ImGui::Image(newMaterial->getPreview(), { 50, 50 });
		ImGui::SameLine();
		ImGui::BeginGroup();
		if (ImGui::MenuItem(FilePath(newMaterial->getFileName()).filename().generic_string().c_str()))
		{
			EventManager::GetSingleton()->call("OpenModel", "EditorOpenFile", newMaterial->getFileName());
		}
		if (ImGui::SmallButton(("Reset##" + oldMaterial->getFileName()).c_str()))
		{
			setMaterialOverride(oldMaterial, oldMaterial);
		}
		ImGui::EndGroup();
		ImGui::EndGroup();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
			{
				const char* payloadFileName = (const char*)payload->Data;
				FilePath payloadPath(payloadFileName);
				if (payloadPath.extension() == ".rmat")
				{
					MaterialLibrary::CreateNewMaterialFile(payloadPath.generic_string(), oldMaterial->getTypeName());
					setMaterialOverride(oldMaterial, MaterialLibrary::GetMaterial(payloadPath.generic_string()));
				}
				else
				{
					WARN("Unsupported file format for material. Use .rmat files");
				}
			}
			ImGui::EndDragDropTarget();
		}		

		ImGui::NextColumn();
		ImGui::Separator();
	}
	ImGui::Columns(1);
}
#endif // ROOTEX_EDITOR
