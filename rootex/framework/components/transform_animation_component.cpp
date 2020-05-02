#include "transform_animation_component.h"

Component* TransformAnimationComponent::Create(const JSON::json& componentData)
{
	Vector<Keyframe> keyframes;

	for (auto& keyframeJSON : componentData["keyframes"])
	{
		Keyframe keyframe;

		keyframe.m_TimePosition = keyframeJSON["timePosition"];
		keyframe.m_Translation = { 
			keyframeJSON["translation"]["x"], 
			keyframeJSON["translation"]["y"], 
			keyframeJSON["translation"]["z"] 
		};
		keyframe.m_Rotation = { 
			keyframeJSON["rotation"]["x"], 
			keyframeJSON["rotation"]["y"], 
			keyframeJSON["rotation"]["z"], 
			keyframeJSON["rotation"]["w"] 
		};
		keyframe.m_Scale = { 
			keyframeJSON["scale"]["x"], 
			keyframeJSON["scale"]["y"], 
			keyframeJSON["scale"]["z"] 
		};

		keyframes.push_back(keyframe);
	}

	return new TransformAnimationComponent(
	    componentData["isPlayOnStart"],
	    componentData["isLooping"],
		keyframes
	);
}

Component* TransformAnimationComponent::CreateDefault()
{
	return new TransformAnimationComponent(
		false,
		false, 
		{ Keyframe({ 0.0f, Vector3::Zero, Quaternion::Identity, Vector3(1.0f, 1.0f, 1.0f) }) });
}

TransformAnimationComponent::TransformAnimationComponent(bool isPlayOnStart, bool isLooping, const Vector<Keyframe>& keyframes)
    : m_CurrentTimePosition(0.0f)
    , m_IsPlaying(isPlayOnStart)
    , m_IsPlayOnStart(isPlayOnStart)
    , m_BasicAnimation({ keyframes })
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

	m_BasicAnimation.m_Keyframes[0].m_Translation = m_InitialPosition;
	m_BasicAnimation.m_Keyframes[0].m_Rotation = m_InitialRotation;
	m_BasicAnimation.m_Keyframes[0].m_Scale = m_InitialScale;
	
	return true;
}

JSON::json TransformAnimationComponent::getJSON() const
{
	JSON::json j;

	for (int i = 0; i < m_BasicAnimation.m_Keyframes.size(); i++)
	{
		j["keyframes"][i]["timePosition"] = m_BasicAnimation.m_Keyframes[i].m_TimePosition;
		j["keyframes"][i]["translation"]["x"] = m_BasicAnimation.m_Keyframes[i].m_Translation.x;
		j["keyframes"][i]["translation"]["y"] = m_BasicAnimation.m_Keyframes[i].m_Translation.y;
		j["keyframes"][i]["translation"]["z"] = m_BasicAnimation.m_Keyframes[i].m_Translation.z;
		j["keyframes"][i]["rotation"]["x"] = m_BasicAnimation.m_Keyframes[i].m_Rotation.x;
		j["keyframes"][i]["rotation"]["y"] = m_BasicAnimation.m_Keyframes[i].m_Rotation.y;
		j["keyframes"][i]["rotation"]["z"] = m_BasicAnimation.m_Keyframes[i].m_Rotation.z;
		j["keyframes"][i]["rotation"]["w"] = m_BasicAnimation.m_Keyframes[i].m_Rotation.w;
		j["keyframes"][i]["scale"]["x"] = m_BasicAnimation.m_Keyframes[i].m_Scale.x;
		j["keyframes"][i]["scale"]["y"] = m_BasicAnimation.m_Keyframes[i].m_Scale.y;
		j["keyframes"][i]["scale"]["z"] = m_BasicAnimation.m_Keyframes[i].m_Scale.z;
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
		for (int i = 0; i < m_BasicAnimation.m_Keyframes.size(); i++)
		{
			Keyframe* keyframe = &m_BasicAnimation.m_Keyframes[i];

			ImGui::InputFloat(("Time##" + std::to_string(i)).c_str(), &keyframe->m_TimePosition);
			ImGui::InputFloat3(("Translation##" + std::to_string(i)).c_str(), &keyframe->m_Translation.x);
			ImGui::InputFloat3(("Rotation##" + std::to_string(i)).c_str(), &keyframe->m_Rotation.x);
			ImGui::InputFloat3(("Scale##" + std::to_string(i)).c_str(), &keyframe->m_Scale.x);
			if (!m_IsPlaying && m_BasicAnimation.m_Keyframes.size() > 1 && ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
			{
				m_BasicAnimation.m_Keyframes.erase(m_BasicAnimation.m_Keyframes.begin() + i);
			}
		}
		if (!m_IsPlaying && ImGui::Button("Add"))
		{
			m_BasicAnimation.m_Keyframes.push_back({ 
				m_BasicAnimation.getEndTime() + 1,
				m_Owner->getComponent<TransformComponent>()->getPosition(),
				m_Owner->getComponent<TransformComponent>()->getRotation(),
				m_Owner->getComponent<TransformComponent>()->getScale()
			});
		}
		ImGui::TreePop();
	}
}
#endif // ROOTEX_EDITOR
