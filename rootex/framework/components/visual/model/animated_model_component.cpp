#include "animated_model_component.h"

#include "core/resource_loader.h"
#include "framework/entity.h"
#include "renderer/render_pass.h"
#include "framework/systems/render_system.h"
#include "renderer/materials/animated_material.h"
#include "renderer/material_library.h"
#include "scene_loader.h"

Ptr<Component> AnimatedModelComponent::Create(const JSON::json& componentData)
{
	return std::make_unique<AnimatedModelComponent>(
	    componentData.value("isPlayOnStart", false),
	    ResourceLoader::CreateAnimatedModelResourceFile(componentData.value("resFile", "rootex/assets/animation.dae")),
	    componentData.value("currentAnimationName", ""),
	    (AnimationMode)componentData.value("animationMode", (int)AnimationMode::None),
	    componentData.value("renderPass", (int)RenderPass::Basic),
	    componentData.value("materialOverrides", HashMap<String, String>()),
	    componentData.value("isVisible", true),
	    componentData.value("lodEnable", true),
	    componentData.value("lodBias", 0.0f),
	    componentData.value("lodDistance", 10.0f),
	    componentData.value("affectingStaticLights", Vector<SceneID>()));
}

AnimatedModelComponent::AnimatedModelComponent(
    bool isPlayOnStart,
    Ref<AnimatedModelResourceFile> resFile,
    const String& currentAnimationName,
    AnimationMode mode,
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
    , m_AnimationMode(mode)
    , m_CurrentAnimationName(currentAnimationName)
{
	assignOverrides(resFile, materialOverrides);
	m_FinalTransforms.resize(m_AnimatedModelResourceFile->getBoneCount());
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

void AnimatedModelComponent::update(float deltaMilliseconds)
{
	switch (m_AnimationMode)
	{
	case AnimationMode::None:
		break;
	case AnimationMode::Looping:
		if (m_CurrentTimePosition > getEndTime())
		{
			m_CurrentTimePosition = 0.0f;
		}
		break;
	case AnimationMode::Alternating:
		if (m_CurrentTimePosition < getStartTime() || getEndTime() < m_CurrentTimePosition)
		{
			m_TimeDirection *= -1.0f;
			m_CurrentTimePosition = std::clamp(m_CurrentTimePosition, getStartTime(), getEndTime());
		}
		deltaMilliseconds *= m_TimeDirection;
		break;
	}
	m_CurrentTimePosition += deltaMilliseconds * MS_TO_S;
	m_RemainingTransitionTime -= deltaMilliseconds * MS_TO_S;
	m_RemainingTransitionTime = std::max(m_RemainingTransitionTime, 0.0f);

	m_AnimatedModelResourceFile->getFinalTransforms(m_FinalTransforms, m_CurrentAnimationName, m_CurrentTimePosition, std::max(0.2f, 1.0f - m_RemainingTransitionTime / m_TransitionTime));
}

void AnimatedModelComponent::setPlaying(bool enabled)
{
	m_IsPlaying = enabled;
}

void AnimatedModelComponent::play()
{
	setPlaying(true);
}

void AnimatedModelComponent::stop()
{
	setPlaying(false);
}

void AnimatedModelComponent::setAnimation(const String& name)
{
	PANIC(m_AnimatedModelResourceFile->getAnimations().find(name) == m_AnimatedModelResourceFile->getAnimations().end(), "Animation name not found: " + name);
	m_CurrentAnimationName = name;
}

void AnimatedModelComponent::transition(const String& name, float transitionTime)
{
	setAnimation(name);
	m_TransitionTime = transitionTime;
	m_RemainingTransitionTime = m_TransitionTime;
}

float AnimatedModelComponent::getStartTime() const
{
	return m_AnimatedModelResourceFile->getAnimationStartTime(m_CurrentAnimationName);
}

float AnimatedModelComponent::getEndTime() const
{
	return m_AnimatedModelResourceFile->getAnimationEndTime(m_CurrentAnimationName);
}

bool AnimatedModelComponent::hasEnded() const
{
	return m_AnimationMode == AnimationMode::None && m_CurrentTimePosition > getEndTime();
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

void AnimatedModelComponent::assignOverrides(Ref<AnimatedModelResourceFile> file, const HashMap<String, String>& materialOverrides)
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

void AnimatedModelComponent::setAnimatedResourceFile(Ref<AnimatedModelResourceFile> resFile, const HashMap<String, String>& materialOverrides)
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
	j["animationMode"] = (int)m_AnimationMode;
	j["transitionTime"] = m_TransitionTime;

	return j;
}

void AnimatedModelComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);
	ImGui::Checkbox("Play on Start", &m_IsPlayOnStart);

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
				setAnimation(animationName);
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Combo("Animation Mode", (int*)&m_AnimationMode, "None\0Looping\0Alternating\0");

	RenderableComponent::draw();
}
