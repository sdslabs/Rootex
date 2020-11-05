#include "renderable_component.h"

#include "static_point_light_component.h"
#include "system.h"
#include "entity_factory.h"
#include "systems\render_system.h"
#include "renderer\material_library.h"

RenderableComponent::RenderableComponent(unsigned int renderPass, const HashMap<String, String>& materialOverrides, bool isVisible, const Vector<EntityID>& affectingStaticLightEntityIDs)
    : m_RenderPass(renderPass)
    , m_IsVisible(isVisible)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
    , m_AffectingStaticLightEntityIDs(affectingStaticLightEntityIDs)
{
}

void RenderableComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<RenderableComponent> renderableComponent = rootex.new_usertype<RenderableComponent>(
		"RenderableComponent",
		sol::base_classes, sol::bases<Component>());
}

bool RenderableComponent::setup()
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

bool RenderableComponent::setupEntities()
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

bool RenderableComponent::preRender(float deltaMilliseconds)
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

bool RenderableComponent::isVisible() const
{
	// TODO: Add culling
	return m_IsVisible;
}

void RenderableComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

void RenderableComponent::setIsVisible(bool enabled)
{
	m_IsVisible = enabled;
}

void RenderableComponent::setMaterialOverride(Ref<Material> oldMaterial, Ref<Material> newMaterial)
{
	m_MaterialOverrides[oldMaterial] = newMaterial;
}

bool RenderableComponent::addAffectingStaticLight(EntityID ID)
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

void RenderableComponent::removeAffectingStaticLight(EntityID ID)
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

void RenderableComponent::render()
{
	PerModelPSCB perModel;
	for (int i = 0; i < m_AffectingStaticLights.size(); i++)
	{
		perModel.staticPointsLightsAffecting[i].id = m_AffectingStaticLights[i];
	}
	perModel.staticPointsLightsAffectingCount = m_AffectingStaticLights.size();
	Material::SetPSConstantBuffer(perModel, m_PerModelCB, PER_MODEL_PS_CPP);
}

JSON::json RenderableComponent::getJSON() const
{
	JSON::json j;

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
void RenderableComponent::draw()
{
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
