#include "animated_model_component.h"

#include "core/resource_loader.h"
#include "framework/entity.h"
#include "renderer/render_pass.h"
#include "framework/systems/render_system.h"
#include "renderer/materials/animated_material.h"
#include "renderer/material_library.h"
#include "scene_loader.h"

AnimatedModelComponent::AnimatedModelComponent(
    bool isPlayOnStart,
    AnimatedModelResourceFile* resFile,
    const String& currentAnimationName,
    unsigned int renderPass,
    const HashMap<String, String>& materialOverrides,
    bool isVisible,
    bool lodEnable,
    float lodBias,
    float lodDistance,
    const Vector<SceneID>& affectingStaticLightIDs)
    : RenderableComponent(renderPass, materialOverrides, isVisible, lodEnable, lodBias, lodDistance, affectingStaticLightIDs)
    , m_CurrentTimePosition(0.0f)
    , m_IsPlaying(isPlayOnStart)
    , m_IsPlayOnStart(isPlayOnStart)
    , m_CurrentAnimationName(currentAnimationName)
{
	assignOverrides(resFile, materialOverrides);
	m_FinalTransforms.resize(m_AnimatedModelResourceFile->getBoneCount());
}

Component* AnimatedModelComponent::Create(const JSON::json& componentData)
{
	AnimatedModelComponent* animatedModelComponent = new AnimatedModelComponent(
	    componentData.value("isPlayOnStart", false),
	    ResourceLoader::CreateAnimatedModelResourceFile(componentData.value("resFile", "rootex/assets/animation.dae")),
	    componentData.value("currentAnimationName", ""),
	    componentData.value("renderPass", (int)RenderPass::Basic),
	    componentData.value("materialOverrides", HashMap<String, String>()),
	    componentData.value("isVisible", true),
	    componentData.value("lodEnable", true),
	    componentData.value("lodBias", 0.0f),
	    componentData.value("lodDistance", 10.0f),
	    componentData.value("affectingStaticLights", Vector<SceneID>()));
	return animatedModelComponent;
}

bool AnimatedModelComponent::setupData()
{
	assignBoundingBox();
	return true;
}

bool AnimatedModelComponent::preRender(float deltaMilliseconds)
{
	ZoneNamedN(componentpreRender, "Animated Model Pre-Render", true);
	RenderableComponent::preRender(deltaMilliseconds);
	return true;
}

void AnimatedModelComponent::render(float viewDistance)
{
	ZoneNamedN(componentRender, "Animated Model Render", true);
	RenderableComponent::render(viewDistance);

	std::sort(m_AnimatedModelResourceFile->getMeshes().begin(), m_AnimatedModelResourceFile->getMeshes().end(), CompareMaterials);

	for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
	{
		(std::dynamic_pointer_cast<AnimatedMaterial>(material))->setVSConstantBuffer(VSAnimationConstantBuffer(m_FinalTransforms));
		RenderSystem::GetSingleton()->getRenderer()->bind(m_MaterialOverrides[material].get());

		for (auto& mesh : meshes)
		{
			RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.getLOD(getLODFactor(viewDistance)).get());
		}
	}
}

void AnimatedModelComponent::assignBoundingBox()
{
	if (m_AnimatedModelResourceFile)
	{
		BoundingBox bigBox;
		bool first = true;
		for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
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

void AnimatedModelComponent::assignOverrides(AnimatedModelResourceFile* file, const HashMap<String, String>& materialOverrides)
{
	m_AnimatedModelResourceFile = file;

	if (!file)
	{
		return;
	}

	m_MaterialOverrides.clear();
	for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
	{
		setMaterialOverride(material, material);
	}
	for (auto& [oldMaterial, newMaterial] : materialOverrides)
	{
		MaterialLibrary::CreateNewMaterialFile(newMaterial, MaterialLibrary::GetMaterial(oldMaterial)->getTypeName());
		setMaterialOverride(MaterialLibrary::GetMaterial(oldMaterial), MaterialLibrary::GetMaterial(newMaterial));
	}
}

void AnimatedModelComponent::setAnimatedResourceFile(AnimatedModelResourceFile* resFile, const HashMap<String, String>& materialOverrides)
{
	if (!resFile)
	{
		WARN("Tried to set a null animated model resource file.");
		return;
	}
	if (resFile->getAnimationNames().size() == 0)
	{
		WARN("No animations found in animation resource file");
		return;
	}
	assignOverrides(resFile, materialOverrides);
	assignBoundingBox();
	m_FinalTransforms.resize(m_AnimatedModelResourceFile->getBoneCount());
	m_CurrentAnimationName = m_AnimatedModelResourceFile->getAnimationNames().front();
}

JSON::json AnimatedModelComponent::getJSON() const
{
	JSON::json j = RenderableComponent::getJSON();

	j["resFile"] = m_AnimatedModelResourceFile->getPath().string();
	j["isPlayOnStart"] = m_IsPlaying;
	j["currentAnimationName"] = m_CurrentAnimationName;

	return j;
}

void AnimatedModelComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);
	ImGui::Checkbox("Play on Start", &m_IsPlayOnStart);

	ImGui::BeginGroup();

	String filePath = m_AnimatedModelResourceFile->getPath().generic_string();
	ImGui::Text("%s", filePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Model"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, m_AnimatedModelResourceFile->getPath().string());
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Animated Model File"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::AnimatedModel), "game/assets/"))
		{
			setAnimatedResourceFile(ResourceLoader::CreateAnimatedModelResourceFile(*result), {});
		}
	}

	if (ImGui::Button("Start"))
	{
		m_IsPlaying = true;
	}
	ImGui::SameLine();
	ImGui::SliderFloat("", &m_CurrentTimePosition, 0.0f, m_AnimatedModelResourceFile->getAnimationEndTime(m_CurrentAnimationName));
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		m_IsPlaying = false;
	}

	if (ImGui::BeginCombo("Animation Name", m_CurrentAnimationName.c_str()))
	{
		for (auto& animationName : m_AnimatedModelResourceFile->getAnimationNames())
		{
			if (ImGui::Selectable(animationName.c_str()))
			{
				m_CurrentAnimationName = animationName;
			}
		}
		ImGui::EndCombo();
	}

	RenderableComponent::draw();

	int p = 0;
	if (ImGui::TreeNodeEx(("Meshes")))
	{
		ImGui::Columns(3);

		ImGui::Text("Serial");
		ImGui::NextColumn();
		ImGui::Text("Vertices");
		ImGui::NextColumn();

		for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
		{
			for (auto& mesh : meshes)
			{
				p++;
				ImGui::Columns(3);
				ImGui::Text("%d", p);
				ImGui::NextColumn();
				ImGui::Text("%d", mesh.m_VertexBuffer->getCount());
				ImGui::NextColumn();
			}

			ImGui::Columns(1);
		}

		ImGui::TreePop();
		ImGui::Separator();
	}
	ImGui::EndGroup();
}
