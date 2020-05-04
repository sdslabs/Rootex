#include "transform_animation_component.h"

Component* TransformAnimationComponent::Create(const JSON::json& componentData)
{
	Vector<TranslationKeyframe> tKeyframes;
	{
		for (auto& keyframeJSON : componentData["keyframes"]["translation"])
		{
			TranslationKeyframe keyframe;

			keyframe.m_TimePosition = keyframeJSON["timePosition"];
			keyframe.m_Translation = {
				keyframeJSON["x"],
				keyframeJSON["y"],
				keyframeJSON["z"]
			};

			tKeyframes.push_back(keyframe);
		}
	}
	Vector<RotationKeyframe> rKeyframes;
	{
		for (auto& keyframeJSON : componentData["keyframes"]["rotation"])
		{
			RotationKeyframe keyframe;

			keyframe.m_TimePosition = keyframeJSON["timePosition"];
			keyframe.m_Rotation = {
				keyframeJSON["x"],
				keyframeJSON["y"],
				keyframeJSON["z"],
				keyframeJSON["w"]
			};

			rKeyframes.push_back(keyframe);
		}
	}
	Vector<ScaleKeyframe> sKeyframes;
	{
		for (auto& keyframeJSON : componentData["keyframes"]["scale"])
		{
			ScaleKeyframe keyframe;

			keyframe.m_TimePosition = keyframeJSON["timePosition"];
			keyframe.m_Scale = {
				keyframeJSON["x"],
				keyframeJSON["y"],
				keyframeJSON["z"]
			};

			sKeyframes.push_back(keyframe);
		}
	}

	return new TransformAnimationComponent(
	    componentData["isPlayOnStart"],
	    componentData["isLooping"],
		tKeyframes,
		rKeyframes,
		sKeyframes
	);
}

Component* TransformAnimationComponent::CreateDefault()
{
	return new TransformAnimationComponent(
		false,
		false, 
		{ TranslationKeyframe({ 0.0f, Vector3::Zero }) }, 
		{ RotationKeyframe({ 0.0f, Quaternion::Identity }) }, 
		{ ScaleKeyframe({ 0.0f, { 1.0f, 1.0f, 1.0f } }) });
}

TransformAnimationComponent::TransformAnimationComponent(bool isPlayOnStart, bool isLooping, const Vector<TranslationKeyframe>& translationKeyframes, const Vector<RotationKeyframe>& rotationKeyframes, const Vector<ScaleKeyframe>& scaleKeyframes)
    : m_CurrentTimePosition(0.0f)
    , m_IsPlaying(isPlayOnStart)
    , m_IsPlayOnStart(isPlayOnStart)
    , m_BasicAnimation({ translationKeyframes, rotationKeyframes, scaleKeyframes })
    , m_IsLooping(isLooping)
{
}

void TransformAnimationComponent::play()
{
	m_IsPlaying = true;
}

void TransformAnimationComponent::pause()
{
	m_IsPlaying = false;
}

void TransformAnimationComponent::reset()
{
	m_CurrentTimePosition = 0.0f;
	m_TransformComponent->setPosition(m_InitialPosition);
	m_TransformComponent->setRotationQuaternion(m_InitialRotation);
	m_TransformComponent->setScale(m_InitialScale);
}

bool TransformAnimationComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();

	if (!m_TransformComponent)
	{
		return false;
	}

	m_InitialPosition = m_TransformComponent->getPosition();
	m_InitialRotation = m_TransformComponent->getRotation();
	m_InitialScale = m_TransformComponent->getScale();

	m_BasicAnimation.m_TranslationKeyframes[0].m_Translation = m_InitialPosition;
	m_BasicAnimation.m_RotationKeyframes[0].m_Rotation = m_InitialRotation;
	m_BasicAnimation.m_ScaleKeyframes[0].m_Scale = m_InitialScale;
	
	return true;
}

JSON::json TransformAnimationComponent::getJSON() const
{
	JSON::json j;

	for (int i = 0; i < m_BasicAnimation.m_TranslationKeyframes.size(); i++)
	{
		j["keyframes"]["translation"][i]["timePosition"] = m_BasicAnimation.m_TranslationKeyframes[i].m_TimePosition;
		j["keyframes"]["translation"][i]["x"] = m_BasicAnimation.m_TranslationKeyframes[i].m_Translation.x;
		j["keyframes"]["translation"][i]["y"] = m_BasicAnimation.m_TranslationKeyframes[i].m_Translation.y;
		j["keyframes"]["translation"][i]["z"] = m_BasicAnimation.m_TranslationKeyframes[i].m_Translation.z;
	}
	for (int i = 0; i < m_BasicAnimation.m_RotationKeyframes.size(); i++)
	{
		j["keyframes"]["rotation"][i]["timePosition"] = m_BasicAnimation.m_RotationKeyframes[i].m_TimePosition;
		j["keyframes"]["rotation"][i]["x"] = m_BasicAnimation.m_RotationKeyframes[i].m_Rotation.x;
		j["keyframes"]["rotation"][i]["y"] = m_BasicAnimation.m_RotationKeyframes[i].m_Rotation.y;
		j["keyframes"]["rotation"][i]["z"] = m_BasicAnimation.m_RotationKeyframes[i].m_Rotation.z;
		j["keyframes"]["rotation"][i]["w"] = m_BasicAnimation.m_RotationKeyframes[i].m_Rotation.w;
	}
	for (int i = 0; i < m_BasicAnimation.m_ScaleKeyframes.size(); i++)
	{
		j["keyframes"]["scale"][i]["timePosition"] = m_BasicAnimation.m_ScaleKeyframes[i].m_TimePosition;
		j["keyframes"]["scale"][i]["x"] = m_BasicAnimation.m_ScaleKeyframes[i].m_Scale.x;
		j["keyframes"]["scale"][i]["y"] = m_BasicAnimation.m_ScaleKeyframes[i].m_Scale.y;
		j["keyframes"]["scale"][i]["z"] = m_BasicAnimation.m_ScaleKeyframes[i].m_Scale.z;
	}

	j["isPlayOnStart"] = m_IsPlayOnStart;
	j["isLooping"] = m_IsLooping;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void TransformAnimationComponent::draw()
{
	ImGui::SliderFloat("Time", &m_CurrentTimePosition, 0.0f, m_BasicAnimation.getEndTime());
	ImGui::SameLine();
	if (ImGui::Checkbox("Play", &m_IsPlaying))
	{
		if (!m_IsPlaying)
		{
			reset();
		}
	}
	
	ImGui::Checkbox("Play On Start", &m_IsPlayOnStart);
	ImGui::Checkbox("Looping", &m_IsLooping);

	if (ImGui::TreeNodeEx("Keyframes"))
	{
		for (int i = 0; i < m_BasicAnimation.m_TranslationKeyframes.size(); i++)
		{
			TranslationKeyframe* keyframe = &m_BasicAnimation.m_TranslationKeyframes[i];

			ImGui::InputFloat(("Time##" + std::to_string(i)).c_str(), &keyframe->m_TimePosition);
			ImGui::InputFloat3(("Translation##" + std::to_string(i)).c_str(), &keyframe->m_Translation.x);
			if (!m_IsPlaying && m_BasicAnimation.m_TranslationKeyframes.size() > 1 && ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
			{
				m_BasicAnimation.m_TranslationKeyframes.erase(m_BasicAnimation.m_TranslationKeyframes.begin() + i);
			}
		}
		if (!m_IsPlaying && ImGui::Button("Add"))
		{
			m_BasicAnimation.m_TranslationKeyframes.push_back({ 
				m_BasicAnimation.getEndTime() + 1,
				m_Owner->getComponent<TransformComponent>()->getPosition()
			});
		}
		ImGui::TreePop();
	}
}
#endif // ROOTEX_EDITOR
