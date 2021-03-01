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
#include "scene_loader.h"

Component* ModelComponent::Create(const JSON::json& componentData)
{
	ModelComponent* modelComponent = new ModelComponent(
	    componentData.value("renderPass", (int)RenderPass::Basic),
	    ResourceLoader::CreateModelResourceFile(componentData.value("resFile", "rootex/assets/cube.obj")),
	    componentData.value("materialOverrides", HashMap<String, String>()),
	    componentData.value("isVisible", true),
	    componentData.value("affectingStaticLights", Vector<SceneID>()));
	return modelComponent;
}

ModelComponent::ModelComponent(unsigned int renderPass, ModelResourceFile* resFile, const HashMap<String, String>& materialOverrides, bool visibility, const Vector<SceneID>& affectingStaticLightIDs)
    : RenderableComponent(renderPass, materialOverrides, visibility, affectingStaticLightIDs)
{
	assignOverrides(resFile, materialOverrides);
}

bool ModelComponent::setupData()
{
	assignBoundingBox();
	return true;
}

bool ModelComponent::preRender(float deltaMilliseconds)
{
	ZoneNamedN(componentPreRender, "Model Pre-Render", true);
	RenderableComponent::preRender(deltaMilliseconds);
	return true;
}

bool ModelComponent::CompareMaterials(const Pair<Ref<Material>, Vector<Mesh>>& a, const Pair<Ref<Material>, Vector<Mesh>>& b)
{
	// Alpha materials final last
	return !a.first->isAlpha() && b.first->isAlpha();
}

void ModelComponent::render()
{
	ZoneNamedN(componentRender, "Model Render", true);
	RenderableComponent::render();

	std::sort(m_ModelResourceFile->getMeshes().begin(), m_ModelResourceFile->getMeshes().end(), CompareMaterials);
	int i = 0;

	RenderableComponent::render();

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

void ModelComponent::setModelResourceFile(ModelResourceFile* newModel, const HashMap<String, String>& materialOverrides)
{
	if (!newModel)
	{
		WARN("Tried to set a null model resource file.");
		return;
	}
	assignOverrides(newModel, materialOverrides);
	assignBoundingBox();
}

void ModelComponent::assignBoundingBox()
{
	if (m_ModelResourceFile)
	{
		BoundingBox bigBox;
		bool first = true;
		for (auto& [material, meshes] : getMeshes())
		{
			for (auto& mesh : meshes)
			{
				if (first)
				{
					bigBox = mesh.m_BoundingBox;
					first = false;
				}
				else
				{
					BoundingBox::CreateMerged(bigBox, bigBox, mesh.m_BoundingBox);
				}
			}
		}
		m_TransformComponent->setBounds(bigBox);
	}
}

void ModelComponent::assignOverrides(ModelResourceFile* newModel, const HashMap<String, String>& materialOverrides)
{
	m_ModelResourceFile = newModel;

	if (!newModel)
	{
		return;
	}

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

JSON::json ModelComponent::getJSON() const
{
	JSON::json j = RenderableComponent::getJSON();

	j["resFile"] = m_ModelResourceFile->getPath().string();

	return j;
}

void ModelComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	String filePath = m_ModelResourceFile->getPath().generic_string();
	ImGui::Text("%s", filePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Model"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, m_ModelResourceFile->getPath().string());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Model File"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Model), "game/assets/"))
		{
			setModelResourceFile(ResourceLoader::CreateModelResourceFile(*result), {});
		}
	}

	RenderableComponent::draw();
}
