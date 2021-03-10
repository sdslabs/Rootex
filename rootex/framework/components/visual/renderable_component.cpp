#include "renderable_component.h"

#include "static_point_light_component.h"
#include "system.h"
#include "systems/render_system.h"
#include "renderer/material_library.h"
#include "scene_loader.h"

RenderableComponent::RenderableComponent(
	unsigned int renderPass,
	const HashMap<String, String>& materialOverrides,
	bool visibility,
    bool lodEnable,
    float lodBias,
    float lodDistance,
    const Vector<SceneID>& affectingStaticLightIDs)
    : m_RenderPass(renderPass)
    , m_IsVisible(visibility)
    , m_AffectingStaticLightIDs(affectingStaticLightIDs)
    , m_LODDistance(lodDistance)
    , m_LODBias(lodBias)
    , m_LODEnable(lodEnable)
    , m_DependencyOnTransformComponent(this)
{
}

float RenderableComponent::getLODFactor(float viewDistance)
{
	return m_LODEnable ? m_LODBias * viewDistance / m_LODDistance : 1.0f;
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

void RenderableComponent::render(float viewDistance)
{
	PerModelPSCB perModel;
	for (int i = 0; i < m_AffectingStaticLights.size(); i++)
	{
		perModel.staticPointsLightsAffecting[i].id = m_AffectingStaticLights[i];
	}
	perModel.staticPointsLightsAffectingCount = m_AffectingStaticLights.size();
	Material::SetPSConstantBuffer(perModel, m_PerModelCB, PER_MODEL_PS_CPP);
}

void RenderableComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

bool RenderableComponent::addAffectingStaticLight(SceneID ID)
{
	Scene* light = SceneLoader::GetSingleton()->getCurrentScene()->findScene(ID);
	if (!light->getEntity())
	{
		WARN("Static light entity referred to not found: " + std::to_string(ID));
		{
			return false;
		};
	}

	int lightID = 0;
	for (auto& component : ECSFactory::GetComponents<StaticPointLightComponent>())
	{
		StaticPointLightComponent* staticLight = (StaticPointLightComponent*)component;
		if (light->getEntity()->getComponent<StaticPointLightComponent>() == staticLight)
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

void RenderableComponent::setVisibility(bool enabled)
{
	m_IsVisible = enabled;
}

void RenderableComponent::setMaterialOverride(Ref<Material> oldMaterial, Ref<Material> newMaterial)
{
	m_MaterialOverrides[oldMaterial] = newMaterial;
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
		ImGui::DragFloat("LOD Bias", &m_LODBias, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("LOD Distance", &m_LODDistance, 1.0f, 0.0f);
		ImGui::Unindent();
	}

	if (ImGui::TreeNodeEx("Static Lights"))
	{
		ImGui::Indent();
		int slot = 0;
		EntityID toRemove = -1;
		for (auto& slotSceneID : m_AffectingStaticLightIDs)
		{
			Scene* staticLight = SceneLoader::GetSingleton()->getCurrentScene()->findScene(slotSceneID);
			RenderSystem::GetSingleton()->submitLine(m_TransformComponent->getAbsoluteTransform().Translation(), staticLight->getEntity()->getComponent<TransformComponent>()->getAbsoluteTransform().Translation());

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
				for (auto& staticLight : ECSFactory::GetComponents<StaticPointLightComponent>())
				{
					if (ImGui::Selectable(staticLight->getOwner()->getFullName().c_str()))
					{
						addAffectingStaticLight(staticLight->getOwner()->getScene()->getID());
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
			ImGui::Text("%s", FilePath(oldMaterial->getFileName()).filename().generic_string().c_str());
			ImGui::NextColumn();
			ImGui::Image(newMaterial->getPreview(), { 50, 50 });
			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::Text("%s", FilePath(newMaterial->getFileName()).filename().generic_string().c_str());
			if (ImGui::Button((ICON_ROOTEX_SEARCH "##" + newMaterial->getFileName()).c_str()))
			{
				EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, newMaterial->getFileName());
			}
			ImGui::SameLine();
			if (ImGui::Button((ICON_ROOTEX_PENCIL_SQUARE_O "##" + newMaterial->getFileName()).c_str()))
			{
				if (Optional<String> result = OS::SelectFile("Material(*.rmat)\0*.rmat\0", "game/assets/materials/"))
				{
					setMaterialOverride(oldMaterial, MaterialLibrary::GetMaterial(*result));
				}
			}
			ImGui::SameLine();
			if (ImGui::Button((ICON_ROOTEX_REFRESH "##" + oldMaterial->getFileName()).c_str()))
			{
				setMaterialOverride(oldMaterial, oldMaterial);
			}
			ImGui::EndGroup();

			ImGui::NextColumn();
			ImGui::Separator();
		}
		ImGui::Columns(1);
		ImGui::Unindent();
		ImGui::TreePop();
	}
}
