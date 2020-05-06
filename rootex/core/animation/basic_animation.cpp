#include "basic_animation.h"

void BasicAnimation::interpolate(Matrix& matrix, float t) const
{
	if (t <= getStartTime())
	{
		matrix = DirectX::XMMatrixAffineTransformation(
		    m_ScaleKeyframes.front().m_Scale,
		    Vector3::Zero,
		    m_RotationKeyframes.front().m_Rotation,
		    m_TranslationKeyframes.front().m_Translation);
	}
	else if (t >= getEndTime())
	{
		matrix = DirectX::XMMatrixAffineTransformation(
		    m_ScaleKeyframes.back().m_Scale,
		    Vector4::Zero,
		    m_RotationKeyframes.back().m_Rotation,
		    m_TranslationKeyframes.back().m_Translation);
	}
	else
	{
		Vector3 translation = Vector3::Zero;
		for (unsigned int i = 0; i < m_TranslationKeyframes.size() - 1; i++)
		{
			if (t > m_TranslationKeyframes[i].m_TimePosition && t < m_TranslationKeyframes[i + 1].m_TimePosition)
			{
				float timeSinceMostRecentKeyframe = t - m_TranslationKeyframes[i].m_TimePosition;
				float timeBetween = m_TranslationKeyframes[i + 1].m_TimePosition - m_TranslationKeyframes[i].m_TimePosition;
				float lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

				translation = Vector3::Lerp(
				    m_TranslationKeyframes[i].m_Translation, 
					m_TranslationKeyframes[i + 1].m_Translation, 
					lerpFactor);
				// No need to check futher. This will be the only one needed.
				break;
			}
		}
		Quaternion rotation = Quaternion::Identity;
		for (unsigned int i = 0; i < m_RotationKeyframes.size() - 1; i++)
		{
			if (t > m_RotationKeyframes[i].m_TimePosition && t < m_RotationKeyframes[i + 1].m_TimePosition)
			{
				float timeSinceMostRecentKeyframe = t - m_RotationKeyframes[i].m_TimePosition;
				float timeBetween = m_RotationKeyframes[i + 1].m_TimePosition - m_RotationKeyframes[i].m_TimePosition;
				float lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

				rotation = Quaternion::Slerp(
				    m_RotationKeyframes[i].m_Rotation,
				    m_RotationKeyframes[i + 1].m_Rotation,
				    lerpFactor);
				// No need to check futher. This will be the only one needed.
				break;
			}
		}
		rotation.Normalize();
		
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
		for (unsigned int i = 0; i < m_ScaleKeyframes.size() - 1; i++)
		{
			if (t > m_ScaleKeyframes[i].m_TimePosition && t < m_ScaleKeyframes[i + 1].m_TimePosition)
			{
				float timeSinceMostRecentKeyframe = t - m_ScaleKeyframes[i].m_TimePosition;
				float timeBetween = m_ScaleKeyframes[i + 1].m_TimePosition - m_ScaleKeyframes[i].m_TimePosition;
				float lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

				scale = Vector3::Lerp(
				    m_ScaleKeyframes[i].m_Scale,
				    m_ScaleKeyframes[i + 1].m_Scale,
				    lerpFactor);

				// No need to check futher. This will be the only one needed.
				break;
			}
		}

		matrix = DirectX::XMMatrixAffineTransformation(
			scale,
			Vector4::Zero,
			rotation,
			translation);
	}
}

float BasicAnimation::getEndTime() const
{
	return max(m_TranslationKeyframes.back().m_TimePosition, max(m_RotationKeyframes.back().m_TimePosition, m_ScaleKeyframes.back().m_TimePosition));
}

float BasicAnimation::getStartTime() const
{
	return min(m_TranslationKeyframes.front().m_TimePosition, min(m_RotationKeyframes.front().m_TimePosition, m_ScaleKeyframes.front().m_TimePosition));
}
