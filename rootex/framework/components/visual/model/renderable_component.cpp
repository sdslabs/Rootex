#include "renderable_component.h"

#include "core/resource_loader.h"
#include "components/visual/light/static_point_light_component.h"
#include "system.h"
#include "systems/render_system.h"
#include "scene_loader.h"

RenderableComponent::RenderableComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_RenderPass(data.value("renderPass", (int)RenderPass::Basic))
    , m_IsVisible(data.value("isVisible", true))
    , m_AffectingStaticLightIDs(data.value("affectingStaticLights", Vector<SceneID>()))
    , m_LODEnable(data.value("lodEnable", false))
    , m_LODBias(data.value("lodBias", 0.0f))
    , m_LODDistance(data.value("lodDistance", 10.0f))
    , m_DependencyOnTransformComponent(this)
{
	m_PerModelCB = RenderingDevice::GetSingleton()->createBuffer<PerModelPSCB>(PerModelPSCB(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

float RenderableComponent::getLODFactor(float viewDistance)
{
	return m_LODEnable ? m_LODBias + m_LODDistance / viewDistance : 1.0f;
}

bool RenderableComponent::setupData()
{
	return true;
}

bool RenderableComponent::setupEntities()
{
	Vector<SceneID> affectingEntities = m_AffectingStaticLightIDs;
	m_AffectingStaticLightIDs.clear();
	m_AffectingStaticLights.clear();
	for (auto& ID : affectingEntities)
	{
		addAffectingStaticLight(ID);
	}
	return true;
}

bool RenderableComponent::preRender(float deltaMilliseconds)
{
	TransformSystem::GetSingleton()->pushMatrixOverride(getTransformComponent()->getAbsoluteTransform());
	return true;
}

void RenderableComponent::render(float viewDistance)
{
	PerModelPSCB perModel;
	for (int i = 0; i < m_AffectingStaticLights.size(); i++)
	{
		perModel.staticPointsLightsAffecting[i].id = m_AffectingStaticLights[i];
	}
	perModel.staticPointsLightsAffectingCount = m_AffectingStaticLights.size();

	RenderingDevice::GetSingleton()->editBuffer(perModel, m_PerModelCB.Get());
	RenderingDevice::GetSingleton()->setPSCB(PER_MODEL_PS_CPP, 1, m_PerModelCB.GetAddressOf());
}

void RenderableComponent::postRender()
{
	TransformSystem::GetSingleton()->popMatrix();
}

bool RenderableComponent::addAffectingStaticLight(SceneID ID)
{
	Scene* light = SceneLoader::GetSingleton()->getCurrentScene()->findScene(ID);

	int lightID = 0;
	for (auto& staticLight : ECSFactory::GetAllStaticPointLightComponent())
	{
		if (light->getEntity().getComponent<StaticPointLightComponent>() == &staticLight)
		{
			m_AffectingStaticLightIDs.push_back(ID);
			m_AffectingStaticLights.push_back(lightID);
			return true;
		}
		lightID++;
	}

	WARN("Provided static light scene does not have a static light: " + light->getFullName());
	return false;
}

void RenderableComponent::removeAffectingStaticLight(SceneID ID)
{
	for (int i = 0; i < m_AffectingStaticLightIDs.size(); i++)
	{
		if (ID == m_AffectingStaticLightIDs[i])
		{
			auto& eraseIt = std::find(m_AffectingStaticLightIDs.begin(), m_AffectingStaticLightIDs.end(), ID);
			m_AffectingStaticLightIDs.erase(eraseIt);

			int removeLightID = m_AffectingStaticLights[i];
			auto& eraseLightIt = std::find(m_AffectingStaticLights.begin(), m_AffectingStaticLights.end(), removeLightID);
			m_AffectingStaticLights.erase(eraseLightIt);
			return;
		}
	}
}

bool RenderableComponent::isVisible() const
{
	// TODO: Add culling
	return m_IsVisible;
}

void RenderableComponent::setVisible(bool enabled)
{
	m_IsVisible = enabled;
}

void RenderableComponent::setMaterialOverride(MaterialResourceFile* oldMaterial, Ref<MaterialResourceFile> newMaterial)
{
	if (oldMaterial && newMaterial)
	{
		m_MaterialOverrides[oldMaterial] = newMaterial;
	}
	else
	{
		WARN("Skipping nullptr material override");
	}
}

JSON::json RenderableComponent::getJSON() const
{
	JSON::json j;

	j["isVisible"] = m_IsVisible;
	j["renderPass"] = m_RenderPass;

	j["materialOverrides"] = {};
	for (auto& [oldMaterial, newMaterial] : m_MaterialOverrides)
	{
		j["materialOverrides"][oldMaterial->getPath().generic_string()] = newMaterial->getPath().generic_string();
	}
	j["affectingStaticLights"] = m_AffectingStaticLightIDs;

	j["lodBias"] = m_LODBias;
	j["lodDistance"] = m_LODDistance;
	j["lodEnable"] = m_LODEnable;

	return j;
}

void RenderableComponent::draw()
{
	int renderPassUI = log2(m_RenderPass);
	if (ImGui::Combo("Renderpass", &renderPassUI, "Basic\0Editor\0Alpha"))
	{
		m_RenderPass = pow(2, renderPassUI);
	}

	ImGui::Checkbox("LOD", &m_LODEnable);
	if (m_LODEnable)
	{
		ImGui::Indent();
		ImGui::DragFloat("LOD Bias", &m_LODBias, 0.01f, 0.0f, FLT_MAX);
		ImGui::DragFloat("LOD Distance", &m_LODDistance, 0.1f, 0.1f, FLT_MAX);
		ImGui::Unindent();
	}

	if (ImGui::TreeNodeEx("Static Lights"))
	{
		ImGui::Indent();
		int slot = 0;
		SceneID toRemove = -1;
		for (auto& slotSceneID : m_AffectingStaticLightIDs)
		{
			Scene* staticLight = SceneLoader::GetSingleton()->getCurrentScene()->findScene(slotSceneID);
			RenderSystem::GetSingleton()->submitLine(getTransformComponent()->getAbsoluteTransform().Translation(), staticLight->getEntity().getComponent<TransformComponent>()->getAbsoluteTransform().Translation());

			String displayName = staticLight->getFullName();
			if (ImGui::SmallButton(("x##" + std::to_string(slot)).c_str()))
			{
				toRemove = slotSceneID;
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
				for (auto& staticLight : ECSFactory::GetAllStaticPointLightComponent())
				{
					if (ImGui::Selectable(staticLight.getOwner().getFullName().c_str()))
					{
						addAffectingStaticLight(staticLight.getOwner().getScene()->getID());
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
			ImGui::Image(oldMaterial->getPreview(), { 50, 50 });
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text("%s", oldMaterial->getPath().filename().generic_string().c_str());
			if (ImGui::Button((ICON_ROOTEX_PENCIL_SQUARE_O "##" + oldMaterial->getPath().generic_string()).c_str()))
			{
				EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { oldMaterial->getPath().generic_string(), (int)oldMaterial->getType() });
			}
			ImGui::EndGroup();
			ImGui::NextColumn();
			ImGui::Image(newMaterial->getPreview(), { 50, 50 });
			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::Text("%s", newMaterial->getPath().filename().generic_string().c_str());
			if (ImGui::Button((ICON_ROOTEX_PENCIL_SQUARE_O "##" + newMaterial->getPath().generic_string()).c_str()))
			{
				EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { newMaterial->getPath().generic_string(), (int)newMaterial->getType() });
			}
			ImGui::SameLine();
			if (ImGui::Button((ICON_ROOTEX_FOLDER_OPEN "##" + newMaterial->getPath().generic_string()).c_str()))
			{
				if (Optional<String> result = OS::SelectFile("Material(*.rmat)\0*.rmat\0", "game/assets/materials/"))
				{
					setMaterialOverride(oldMaterial, ResourceLoader::CreateMaterialResourceFile(*result));
				}
			}
			ImGui::SameLine();
			if (ImGui::Button((ICON_ROOTEX_REFRESH "##" + oldMaterial->getPath().generic_string()).c_str()))
			{
				if (Optional<String> result = OS::SelectFile("Material(*.rmat)\0*.rmat\0", "game/assets/materials/"))
				{
					setMaterialOverride(oldMaterial, ResourceLoader::CreateMaterialResourceFile(*result));
				}
			}
			ImGui::EndGroup();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL_PAYLOAD"))
				{
					const char* path = (const char*)payload->Data;
					setMaterialOverride(oldMaterial, ResourceLoader::CreateMaterialResourceFile(path));
				}
			}
			ImGui::NextColumn();
			ImGui::Separator();
		}
		ImGui::Columns(1);
		ImGui::Unindent();
		ImGui::TreePop();
	}
}
