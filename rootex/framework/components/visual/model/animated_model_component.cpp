#include "animated_model_component.h"

#include "core/resource_loader.h"
#include "framework/entity.h"
#include "renderer/render_pass.h"
#include "framework/systems/render_system.h"
#include "scene_loader.h"

DEFINE_COMPONENT(AnimatedModelComponent);

AnimatedModelComponent::AnimatedModelComponent(Entity& owner, const JSON::json& data)
    : RenderableComponent(owner, data)
    , m_RootExclusion(data.value("rootExclusion", RootExclusion::None))
    , m_SpeedMultiplier(data.value("speedMultiplier", 1.0f))
    , m_IsPlaying(data.value("isPlayOnStart", false))
    , m_IsPlayOnStart(data.value("isPlayOnStart", false))
    , m_AnimationMode((AnimationMode)data.value("animationMode", (int)AnimationMode::None))
    , m_CurrentAnimationName(data.value("currentAnimationName", ""))
    , m_CurrentTimePosition(0.0f)
{
	assignOverrides(ResourceLoader::CreateAnimatedModelResourceFile(data.value("resFile", "rootex/assets/animation.dae")), data.value("materialOverrides", HashMap<String, String>()));
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

	bool uploadBones = true;
	for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
	{
		if (Ref<AnimatedBasicMaterialResourceFile> overridingMaterial = std::dynamic_pointer_cast<AnimatedBasicMaterialResourceFile>(m_MaterialOverrides[material]))
		{
			if (uploadBones)
			{
				overridingMaterial->uploadAnimationBuffer(PerModelAnimationVSCBData(m_FinalTransforms));
				uploadBones = false;
			}
			RenderSystem::GetSingleton()->getRenderer()->bind(overridingMaterial.get());

			for (auto& mesh : meshes)
			{
				RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.getLOD(getLODFactor(viewDistance)).get());
			}
		}
	}
}

void AnimatedModelComponent::checkCurrentAnimationExists()
{
	const HashMap<String, SkeletalAnimation>& animations = m_AnimatedModelResourceFile->getAnimations();
	if (animations.find(m_CurrentAnimationName) == animations.end())
	{
		WARN("Animation " + m_CurrentAnimationName + " doesn't exist on " + m_AnimatedModelResourceFile->getPath().generic_string());
		setAnimation(animations.begin()->first);
	}
}

void AnimatedModelComponent::update(float deltaMilliseconds)
{
	checkCurrentAnimationExists();

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
	m_CurrentTimePosition += deltaMilliseconds * m_SpeedMultiplier * MS_TO_S;
	m_RemainingTransitionTime -= deltaMilliseconds * MS_TO_S;
	m_RemainingTransitionTime = std::max(m_RemainingTransitionTime, 0.0f);

	m_AnimatedModelResourceFile->getFinalTransforms(m_FinalTransforms, m_CurrentAnimationName, m_CurrentTimePosition, std::max(0.2f, 1.0f - m_RemainingTransitionTime / m_TransitionTime), m_RootExclusion);
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
	swapAnimation(name);
	m_CurrentTimePosition = 0.0f;
}

void AnimatedModelComponent::swapAnimation(const String& name)
{
	PANIC(m_AnimatedModelResourceFile->getAnimations().find(name) == m_AnimatedModelResourceFile->getAnimations().end(), "Animation name not found: " + name);
	m_CurrentAnimationName = name;
}

void AnimatedModelComponent::transition(const String& name, float transitionTime)
{
	swapTransition(name, transitionTime);
	m_CurrentTimePosition = 0.0f;
}

void AnimatedModelComponent::swapTransition(const String& name, float transitionTime)
{
	swapAnimation(name);
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
		getTransformComponent()->setBounds(bigBox);
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
		setMaterialOverride(
		    ResourceLoader::CreateNewAnimatedBasicMaterialResourceFile(oldMaterial),
		    ResourceLoader::CreateNewAnimatedBasicMaterialResourceFile(newMaterial));
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
	j["speedMultiplier"] = m_SpeedMultiplier;
	j["rootExclusion"] = m_RootExclusion;

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
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_AnimatedModelResourceFile->getPath().generic_string(), (int)m_AnimatedModelResourceFile->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Animated Model File"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::AnimatedModel), "game/assets/"))
		{
			setAnimatedResourceFile(ResourceLoader::CreateAnimatedModelResourceFile(*result), {});
		}
	}

	if (ImGui::Button("Start"))
	{
		play();
	}
	ImGui::SameLine();
	ImGui::SliderFloat("", &m_CurrentTimePosition, 0.0f, m_AnimatedModelResourceFile->getAnimationEndTime(m_CurrentAnimationName));
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		stop();
	}

	ImGui::DragFloat("Speed Multiplier", &m_SpeedMultiplier, 0.01f, 0.0f, 10.0f);

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
	ImGui::Combo("Root Exclusion", (int*)&m_RootExclusion, "None\0Translation\0All\0");

	RenderableComponent::draw();
}
