#include "model_component.h"

#include "common/common.h"

#include "core/resource_loader.h"
#include "event_manager.h"
#include "framework/entity.h"
#include "framework/systems/light_system.h"
#include "framework/systems/render_system.h"
#include "framework/components/visual/static_point_light_component.h"
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
	Vector<EntityID> affectingStaticLights;
	if (componentData.find("affectingStaticLights") != componentData.end())
	{
		for (int lightEntityID : componentData["affectingStaticLights"])
		{
			affectingStaticLights.push_back(lightEntityID);
		}
	}
	ModelComponent* modelComponent = new ModelComponent(
	    componentData["renderPass"],
	    ResourceLoader::CreateModelResourceFile(componentData["resFile"]),
	    materialOverrides,
	    componentData["isVisible"],
	    affectingStaticLights);

	return modelComponent;
}

Component* ModelComponent::CreateDefault()
{
	ModelComponent* modelComponent = new ModelComponent(
	    (int)RenderPass::Basic,
	    ResourceLoader::CreateModelResourceFile("rootex/assets/cube.obj"),
	    {},
	    true,
	    {});

	return modelComponent;
}

ModelComponent::ModelComponent(unsigned int renderPass, ModelResourceFile* resFile, const HashMap<String, String>& materialOverrides, bool visibility, const Vector<EntityID>& affectingStaticLightIDs)
    : m_IsVisible(visibility)
    , m_RenderPass(renderPass)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
    , m_AffectingStaticLightEntityIDs(affectingStaticLightIDs)
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

bool ModelComponent::setupEntities()
{
	Vector<int> affectingEntities = m_AffectingStaticLightEntityIDs;
	m_AffectingStaticLightEntityIDs.clear();
	m_AffectingStaticLights.clear();
	for (auto& ID : affectingEntities)
	{
		addAffectingStaticLight(ID);
	}
	return true;
}

bool ModelComponent::addAffectingStaticLight(EntityID ID)
{
	Ref<Entity> entity = EntityFactory::GetSingleton()->findEntity(ID);
	if (!entity)
	{
		WARN("Static light entity referred to not found: " + std::to_string(ID));
		{
			return false;
		};
	}

	int lightID = 0;
	for (auto& component : System::GetComponents(StaticPointLightComponent::s_ID))
	{
		if (entity->getComponent<StaticPointLightComponent>().get() == component)
		{
			m_AffectingStaticLightEntityIDs.push_back(ID);
			m_AffectingStaticLights.push_back(lightID);
			return true;
		}
		lightID++;
	}
	WARN("Provided static light entity does not have a static light: " + entity->getFullName());
	return false;
}

void ModelComponent::removeAffectingStaticLight(EntityID ID)
{
	for (int i = 0; i < m_AffectingStaticLightEntityIDs.size(); i++)
	{
		if (ID == m_AffectingStaticLightEntityIDs[i])
		{
			auto& eraseIt = std::find(m_AffectingStaticLightEntityIDs.begin(), m_AffectingStaticLightEntityIDs.end(), ID);
			m_AffectingStaticLightEntityIDs.erase(eraseIt);

			int removeLightID = m_AffectingStaticLights[i];
			auto& eraseLightIt = std::find(m_AffectingStaticLights.begin(), m_AffectingStaticLights.end(), removeLightID);
			m_AffectingStaticLights.erase(eraseLightIt);
			return;
		}
	}
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

bool CompareMaterials(const Pair<Ref<Material>, Vector<Mesh>>& a, const Pair<Ref<Material>, Vector<Mesh>>& b)
{
	// Alpha materials final last
	return !a.first->isAlpha() && b.first->isAlpha();
}

void ModelComponent::render()
{
	std::sort(m_ModelResourceFile->getMeshes().begin(), m_ModelResourceFile->getMeshes().end(), CompareMaterials);
	int i = 0;

	PerModelPSCB perModel;
	for (int i = 0; i < m_AffectingStaticLights.size(); i++)
	{
		perModel.staticPointsLightsAffecting[i].id = m_AffectingStaticLights[i];
	}
	perModel.staticPointsLightsAffectingCount = m_AffectingStaticLights.size();
	Material::SetPSConstantBuffer(perModel, m_PerModelCB, PER_MODEL_PS_CPP);
	
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
	j["affectingStaticLights"] = m_AffectingStaticLightEntityIDs;

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

	if (ImGui::TreeNodeEx("Static Lights"))
	{
		ImGui::Indent();
		int slot = 0;
		EntityID toRemove = -1;
		for (auto& slotEntityID : m_AffectingStaticLightEntityIDs)
		{
			Ref<Entity> staticLight = EntityFactory::GetSingleton()->findEntity(slotEntityID);
			RenderSystem::GetSingleton()->submitLine(m_TransformComponent->getAbsoluteTransform().Translation(), staticLight->getComponent<TransformComponent>()->getAbsoluteTransform().Translation());

			String displayName = staticLight->getFullName();
			if (ImGui::SmallButton(("x##" + std::to_string(slot)).c_str()))
			{
				toRemove = slotEntityID;
			}
			ImGui::SameLine();
			ImGui::Text("%s", displayName.c_str());
			slot++;
		}

		if (toRemove != -1)
		{
			removeAffectingStaticLight(toRemove);
		}

		if (slot < MAX_STATIC_POINT_LIGHTS_AFFECTING_1_OBJECT)
		{
			if (ImGui::BeginCombo(("Light " + std::to_string(slot)).c_str(), "None"))
			{
				for (auto& component : System::GetComponents(StaticPointLightComponent::s_ID))
				{
					if (ImGui::Selectable(component->getOwner()->getFullName().c_str()))
					{
						addAffectingStaticLight(component->getOwner()->getID());
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::Unindent();
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Materials"))
	{
		ImGui::Indent();
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
		ImGui::Unindent();
		ImGui::TreePop();
	}
}
#endif // ROOTEX_EDITOR
