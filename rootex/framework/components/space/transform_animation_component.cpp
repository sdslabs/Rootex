#include "transform_animation_component.h"

#include "transform_component.h"
#include "entity.h"
#include "systems/render_system.h"
#include "utility/maths.h"

void to_json(JSON::json& j, const TransformAnimationComponent::Keyframe& k)
{
	j["timePosition"] = k.timePosition;
	j["transform"] = k.transform;
}

void from_json(const JSON::json& j, TransformAnimationComponent::Keyframe& k)
{
	k.timePosition = j.at("timePosition");
	k.transform = j.at("transform");
}

TransformAnimationComponent::TransformAnimationComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_Keyframes(data.value("keyframes", Vector<TransformAnimationComponent::Keyframe>()))
    , m_IsPlayOnStart(data.value("isPlayOnStart", false))
    , m_AnimationMode((AnimationMode)(int)data.value("animationMode", (int)AnimationMode::None))
    , m_TransitionType((TransitionType)(int)data.value("transitionType", (int)TransitionType::SmashSmash))
    , m_IsPlaying(false)
    , m_CurrentTimePosition(0.0f)
    , m_TimeDirection(1.0f)
    , m_DependencyOnTransformComponent(this)
{
}

bool TransformAnimationComponent::setupData()
{
	if (m_Keyframes.empty())
	{
		Keyframe initialKeyframe;
		initialKeyframe.timePosition = 0.0f;
		initialKeyframe.transform = getTransformComponent()->getLocalTransform();
		m_Keyframes.push_back(initialKeyframe);
	}
	return true;
}

void TransformAnimationComponent::pushKeyframe(const Keyframe& keyFrame)
{
	m_Keyframes.push_back(keyFrame);
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
	return m_AnimationMode == AnimationMode::None && m_CurrentTimePosition > getEndTime();
}

float TransformAnimationComponent::getStartTime() const
{
	return m_Keyframes.front().timePosition;
}

float TransformAnimationComponent::getEndTime() const
{
	return m_Keyframes.back().timePosition;
}

void TransformAnimationComponent::reset()
{
	m_CurrentTimePosition = 0.0f;
	interpolate(0.0f);
}

void TransformAnimationComponent::interpolate(float deltaSeconds)
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
		deltaSeconds *= m_TimeDirection;
		break;
	}
	m_CurrentTimePosition += deltaSeconds;

	if (m_CurrentTimePosition <= getStartTime())
	{
		getTransformComponent()->setTransform(m_Keyframes.front().transform);
	}
	else if (m_CurrentTimePosition >= getEndTime())
	{
		getTransformComponent()->setTransform(m_Keyframes.back().transform);
	}
	else
	{
		for (unsigned int i = 0; i < m_Keyframes.size() - 1; i++)
		{
			if (m_CurrentTimePosition > m_Keyframes[i].timePosition && m_CurrentTimePosition < m_Keyframes[i + 1u].timePosition)
			{
				float timeSinceMostRecentKeyframe = m_CurrentTimePosition - m_Keyframes[i].timePosition;
				float timeBetween = m_Keyframes[i + 1u].timePosition - m_Keyframes[i].timePosition;
				float lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

				switch (m_TransitionType)
				{
				case TransitionType::SmashSmash:
					lerpFactor = lerpFactor;
					break;
				case TransitionType::EaseEase:
					lerpFactor = 0.5f * (sin(DirectX::g_XMPi[0] * (lerpFactor - 0.5f)) + 1);
					break;
				case TransitionType::SmashEase:
					lerpFactor = sin(DirectX::g_XMPi[0] * lerpFactor / 2.0f);
					break;
				case TransitionType::EaseSmash:
					lerpFactor = 1.0f - sin(DirectX::g_XMPi[0] * (1.0f - lerpFactor) / 2.0f);
					break;
				}

				Matrix& leftMat = m_Keyframes[i].transform;
				Matrix& rightMat = m_Keyframes[i + 1u].transform;

				Matrix finalMat = Interpolate(leftMat, rightMat, lerpFactor);
				getTransformComponent()->setTransform(finalMat);

				// No need to check futher. This will be the only one needed.
				break;
			}
		}
	}
}

void TransformAnimationComponent::setPlaying(bool enabled)
{
	m_IsPlaying = enabled;
}

JSON::json TransformAnimationComponent::getJSON() const
{
	JSON::json j;

	j["keyframes"] = m_Keyframes;
	j["isPlayOnStart"] = m_IsPlayOnStart;
	j["animationMode"] = m_AnimationMode;
	j["transitionType"] = (int)m_TransitionType;

	return j;
}

void TransformAnimationComponent::draw()
{
	ImGui::Combo("Transition Type", (int*)&m_TransitionType, "SmashSmash\0EaseEase\0SmashEase\0EaseSmash\0");
	ImGui::Combo("Animation Mode", (int*)&m_AnimationMode, "None\0Looping\0Alternating\0");

	ImGui::SliderFloat("Time", &m_CurrentTimePosition, 0.0f, getEndTime());
	if (ImGui::Button("Reset##Component"))
	{
		reset();
	}
	ImGui::Checkbox("Play In Editor", &m_IsPlaying);
	ImGui::Checkbox("Play On Start", &m_IsPlayOnStart);

	static bool isJumping = false;
	static Keyframe* jumpingOn = nullptr;

	if (ImGui::TreeNodeEx("Keyframes"))
	{
		for (int i = 0; i < m_Keyframes.size() - 1; i++)
		{
			RenderSystem::GetSingleton()->submitLine(
			    (getTransformComponent()->getParentAbsoluteTransform() * m_Keyframes[i].transform).Translation(),
			    (getTransformComponent()->getParentAbsoluteTransform() * m_Keyframes[i + 1u].transform).Translation());
		}

		unsigned int keyFrameNumber = 1;
		for (auto& keyframe : m_Keyframes)
		{
			ImGui::DragFloat(("Time##" + std::to_string(keyFrameNumber)).c_str(), &keyframe.timePosition);
			ImGui::SameLine();
			if (isJumping && jumpingOn == &keyframe)
			{
				if (ImGui::Button(("Exit Jump##" + std::to_string(keyFrameNumber)).c_str()))
				{
					isJumping = false;
					jumpingOn = nullptr;
				}
			}
			else
			{
				if (ImGui::Button(("Jump##" + std::to_string(keyFrameNumber)).c_str()))
				{
					isJumping = true;
					jumpingOn = &keyframe;
					getTransformComponent()->setTransform(keyframe.transform);
				}
			}
			ImGui::Separator();
			keyFrameNumber++;
		}

		if (!isJumping)
		{
			if (ImGui::Button("Set Keyframe"))
			{
				Keyframe newKeyframe;
				newKeyframe.timePosition = m_Keyframes.back().timePosition + 1.0f;
				newKeyframe.transform = getTransformComponent()->getLocalTransform();
				pushKeyframe(newKeyframe);
			}
			if (m_Keyframes.size() > 1)
			{
				ImGui::SameLine();
				if (ImGui::Button("Pop keyframe"))
				{
					popKeyframe(1);
				}
			}
		}

		ImGui::TreePop();
	}

	if (isJumping)
	{
		jumpingOn->transform = getTransformComponent()->getLocalTransform();
	}
}
