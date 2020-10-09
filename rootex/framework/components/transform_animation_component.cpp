#include "transform_animation_component.h"
#include "transform_component.h"
#include "entity.h"
#include "systems/render_system.h"

Component* TransformAnimationComponent::Create(const JSON::json& componentData)
{
	Vector<Keyframe> keyframes;
	for (auto& keyframeInfo : componentData["keyframes"])
	{
		Matrix transform;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				transform.m[i][j] = keyframeInfo["transform"][i * 4 + j];
			}
		}
		keyframes.push_back({ keyframeInfo["timePosition"], transform });
	}
	TransitionType transition = TransitionType::SmashSmash;
	if (componentData.find("transitionType") != componentData.end())
	{
		transition = (TransitionType)(int)componentData["transitionType"];
	}
	AnimationMode animationMode = AnimationMode::None;
	if (componentData.find("animationMode") != componentData.end())
	{
		animationMode = componentData["animationMode"];
	}
	TransformAnimationComponent* animation = new TransformAnimationComponent(keyframes, componentData["isPlayOnStart"], animationMode, transition);
	return animation;
}

Component* TransformAnimationComponent::CreateDefault()
{
	return new TransformAnimationComponent({ TransformAnimationComponent::Keyframe({ 0.0f, Matrix::Identity }) }, false, AnimationMode::None, TransitionType::SmashSmash);
}

TransformAnimationComponent::TransformAnimationComponent(const Vector<Keyframe> keyframes, bool isPlayOnStart, AnimationMode animationMode, TransitionType transition)
    : m_Keyframes(keyframes)
    , m_CurrentTimePosition(0.0f)
    , m_IsPlayOnStart(isPlayOnStart)
    , m_AnimationMode(animationMode)
    , m_TransformComponent(nullptr)
    , m_IsPlaying(false)
    , m_TransitionType(transition)
    , m_TimeDirection(1.0f)
{
}

Matrix TransformAnimationComponent::interpolateMatrix(const Matrix& left, const Matrix& right, float lerpFactor)
{
	Quaternion leftRotation = Quaternion::CreateFromRotationMatrix(left);
	Quaternion rightRotation = Quaternion::CreateFromRotationMatrix(right);

	Quaternion finalRotation = Quaternion::Slerp(leftRotation, rightRotation, lerpFactor);

	Matrix finalMat = Matrix::CreateFromQuaternion(finalRotation);

	for (int i : { 0, 1, 2 })
	{
		finalMat.m[3][i] = left.m[3][i] * (1.0f - lerpFactor) + right.m[3][i] * lerpFactor;
	}

	return finalMat;
}

bool TransformAnimationComponent::setup()
{
	m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
	if (!m_TransformComponent)
	{
		WARN("TransformComponent not found on entity with TransformAnimationComponent: " + m_Owner->getFullName());
		return false;
	}
	if (m_Keyframes.empty())
	{
		Keyframe initialKeyframe;
		initialKeyframe.timePosition = 0.0f;
		initialKeyframe.transform = m_TransformComponent->getLocalTransform();
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
		}
		deltaSeconds *= m_TimeDirection;
		break;
	}
	m_CurrentTimePosition += deltaSeconds;

	if (m_CurrentTimePosition <= getStartTime())
	{
		m_TransformComponent->setTransform(m_Keyframes.front().transform);
	}
	else if (m_CurrentTimePosition >= getEndTime())
	{
		m_TransformComponent->setTransform(m_Keyframes.back().transform);
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

				const Matrix& leftMat = m_Keyframes[i].transform;
				const Matrix& rightMat = m_Keyframes[i + 1u].transform;

				Matrix finalMat = interpolateMatrix(leftMat, rightMat, lerpFactor);
				m_TransformComponent->setTransform(finalMat);
				
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

void TransformAnimationComponent::onTrigger()
{
	setPlaying(true);
}

JSON::json TransformAnimationComponent::getJSON() const
{
	JSON::json j;

	for (int i = 0; i < m_Keyframes.size(); i++)
	{
		j["keyframes"][i]["timePosition"] = m_Keyframes[i].timePosition;
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				j["keyframes"][i]["transform"][x * 4u + y] = m_Keyframes[i].transform.m[x][y];
			}
		}
	}

	j["isPlayOnStart"] = m_IsPlayOnStart;
	j["animationMode"] = m_AnimationMode;
	j["transitionType"] = (int)m_TransitionType;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
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
			    (m_TransformComponent->getParentAbsoluteTransform() * m_Keyframes[i].transform).Translation(),
			    (m_TransformComponent->getParentAbsoluteTransform() * m_Keyframes[i + 1u].transform).Translation());
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
					m_TransformComponent->setTransform(keyframe.transform);
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
				newKeyframe.transform = m_TransformComponent->getLocalTransform();
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
		jumpingOn->transform = m_TransformComponent->getLocalTransform();
	}
}
#endif // ROOTEX_EDITOR
