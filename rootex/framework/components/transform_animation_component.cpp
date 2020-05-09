#include "transform_animation_component.h"
#include "transform_component.h"
#include "entity.h"

Component* TransformAnimationComponent::Create(const JSON::json& componentData)
{
	Vector<Keyframe> keyframes;

	for (auto& keyframeInfo : componentData["keyframes"])
	{
		keyframes.push_back({ 
			keyframeInfo["timePosition"], 
			{ 
				keyframeInfo["translation"]["x"],
		        keyframeInfo["translation"]["y"],
		        keyframeInfo["translation"]["z"],
			},
		    {
		        keyframeInfo["rotation"]["x"],
		        keyframeInfo["rotation"]["y"],
		        keyframeInfo["rotation"]["z"],
		        keyframeInfo["rotation"]["w"],
			},
		    {
		        keyframeInfo["scale"]["x"],
		        keyframeInfo["scale"]["y"],
		        keyframeInfo["scale"]["z"],
		    } 
		});
	}

	TransformAnimationComponent* animation = new TransformAnimationComponent(keyframes, componentData["isPlayOnStart"], componentData["isLooping"]);
	return animation;
}

Component* TransformAnimationComponent::CreateDefault()
{
	return new TransformAnimationComponent(
	    { 
			{ 0.0f, Vector3::Zero, Quaternion::Identity, { 1.0f, 1.0f, 1.0f } } 
		}, 
		true,
		false
	);
}

TransformAnimationComponent::TransformAnimationComponent(const Vector<Keyframe> keyframes, bool isPlayOnStart, bool isLooping)
    : m_Keyframes(keyframes)
    , m_CurrentTimePosition(0.0f)
    , m_TransformComponent(nullptr)
    , m_IsPlayOnStart(isPlayOnStart)
    , m_IsLooping(isLooping)
{
	m_IsPlaying = m_IsPlayOnStart;
}

bool TransformAnimationComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	if (!m_TransformComponent)
	{
		WARN("TransformComponent not found on entity with TransformAnimationComponent: " + m_Owner->getFullName());
		return false;
	}

	m_InitialPosition = m_TransformComponent->getPosition();
	m_InitialRotation = m_TransformComponent->getRotation();
	m_InitialScale = m_TransformComponent->getScale();

	m_Keyframes.front().m_Translation = m_InitialPosition;
	m_Keyframes.front().m_Rotation = m_InitialRotation;
	m_Keyframes.front().m_Scale = m_InitialScale;

	return true;
}

void TransformAnimationComponent::pushKeyframe(float timePosition, const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	m_Keyframes.push_back({ timePosition, position, rotation, scale });
}

void TransformAnimationComponent::popKeyframe(int count)
{
	for (int i = 0; i < count; i++)
	{
		m_Keyframes.pop_back();
	}
}

bool TransformAnimationComponent::hasEnded() const
{
	return m_CurrentTimePosition > getEndTime();
}

float TransformAnimationComponent::getStartTime() const
{
	return m_Keyframes.front().m_TimePosition;
}

float TransformAnimationComponent::getEndTime() const
{
	return m_Keyframes.back().m_TimePosition;
}

void TransformAnimationComponent::interpolate(float t)
{
	if (t <= getStartTime())
	{
		m_TransformComponent->setTransform(DirectX::XMMatrixAffineTransformation(
		    m_Keyframes.front().m_Scale,
		    Vector3::Zero,
		    m_Keyframes.front().m_Rotation,
		    m_Keyframes.front().m_Translation));
	}
	else if (t >= getEndTime())
	{
		m_TransformComponent->setTransform(DirectX::XMMatrixAffineTransformation(
		    m_Keyframes.back().m_Scale,
		    Vector4::Zero,
		    m_Keyframes.back().m_Rotation,
		    m_Keyframes.back().m_Translation));
	}
	else
	{
		Vector3 translation = Vector3::Zero;
		Quaternion rotation = Quaternion::Identity;
		Vector3 scale = { 1.0f, 1.0f, 1.0f };

		for (unsigned int i = 0; i < m_Keyframes.size() - 1; i++)
		{
			if (t > m_Keyframes[i].m_TimePosition && t < m_Keyframes[i + 1].m_TimePosition)
			{
				float timeSinceMostRecentKeyframe = t - m_Keyframes[i].m_TimePosition;
				float timeBetween = m_Keyframes[i + 1].m_TimePosition - m_Keyframes[i].m_TimePosition;
				float lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

				translation = Vector3::Lerp(
				    m_Keyframes[i].m_Translation,
				    m_Keyframes[i + 1].m_Translation,
				    lerpFactor);
				rotation = Quaternion::Slerp(
				    m_Keyframes[i].m_Rotation,
				    m_Keyframes[i + 1].m_Rotation,
				    lerpFactor);
				scale = Vector3::Lerp(
				    m_Keyframes[i].m_Scale,
				    m_Keyframes[i + 1].m_Scale,
				    lerpFactor);
		
				m_TransformComponent->setTransform(DirectX::XMMatrixAffineTransformation(
					scale,
					Vector4::Zero,
					rotation,
					translation));
				
				// No need to check futher. This will be the only one needed.
				break;
			}
		}

	}
}

JSON::json TransformAnimationComponent::getJSON() const
{
	JSON::json j;

	for (int i = 0; i < m_Keyframes.size(); i++)
	{
		j["keyframes"][i]["timePosition"] = m_Keyframes[i].m_TimePosition;
		j["keyframes"][i]["translation"]["x"] = m_Keyframes[i].m_Translation.x;
		j["keyframes"][i]["translation"]["y"] = m_Keyframes[i].m_Translation.y;
		j["keyframes"][i]["translation"]["z"] = m_Keyframes[i].m_Translation.z;
		j["keyframes"][i]["rotation"]["x"] = m_Keyframes[i].m_Rotation.x;
		j["keyframes"][i]["rotation"]["y"] = m_Keyframes[i].m_Rotation.y;
		j["keyframes"][i]["rotation"]["z"] = m_Keyframes[i].m_Rotation.z;
		j["keyframes"][i]["rotation"]["w"] = m_Keyframes[i].m_Rotation.w;
		j["keyframes"][i]["scale"]["x"] = m_Keyframes[i].m_Scale.x;
		j["keyframes"][i]["scale"]["y"] = m_Keyframes[i].m_Scale.y;
		j["keyframes"][i]["scale"]["z"] = m_Keyframes[i].m_Scale.z;
	}

	j["isPlayOnStart"] = m_IsPlayOnStart;
	j["isLooping"] = m_IsLooping;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void TransformAnimationComponent::draw()
{
	ImGui::SliderFloat("Time", &m_CurrentTimePosition, 0.0f, getEndTime());
	if (ImGui::Checkbox("Play In Editor", &m_IsPlaying))
	{
		if (!m_IsPlaying)
		{
			interpolate(0.0f);
			setup();
		}
	}
	ImGui::Checkbox("Loop", &m_IsLooping);
	ImGui::Checkbox("Play On Start", &m_IsPlayOnStart);

	if (ImGui::TreeNodeEx("Keyframes"))
	{
		for (auto& keyframe : m_Keyframes)
		{
			ImGui::InputFloat(("Time##" + std::to_string(keyframe.m_TimePosition)).c_str(), &keyframe.m_TimePosition);
			ImGui::InputFloat3(("Translation##" + std::to_string(keyframe.m_TimePosition)).c_str(), &keyframe.m_Translation.x);
			ImGui::InputFloat4(("Rotation##" + std::to_string(keyframe.m_TimePosition)).c_str(), &keyframe.m_Rotation.x);
			ImGui::InputFloat3(("Scale##" + std::to_string(keyframe.m_TimePosition)).c_str(), &keyframe.m_Scale.x);
			ImGui::Separator();
		}

		if (ImGui::Button("+"))
		{
			pushKeyframe(m_Keyframes.back().m_TimePosition + 1.0f, m_TransformComponent->getPosition(), m_TransformComponent->getRotation(), m_TransformComponent->getScale());
		}
		if (m_Keyframes.size() > 1)
		{
			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				popKeyframe(1);
			}
		}

		ImGui::TreePop();
	}
}
#endif // ROOTEX_EDITOR
