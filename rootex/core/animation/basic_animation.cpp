#include "basic_animation.h"

void BasicAnimation::interpolate(Matrix& matrix, float t)
{
	if (t <= getStartTime())
	{
		matrix = DirectX::XMMatrixAffineTransformation(
		    m_Keyframes.front().m_Scale,
		    Vector3::Zero,
		    m_Keyframes.front().m_Rotation,
		    m_Keyframes.front().m_Translation);
	}
	else if (t >= getEndTime())
	{
		matrix = DirectX::XMMatrixAffineTransformation(
		    m_Keyframes.back().m_Scale,
		    Vector4::Zero,
		    m_Keyframes.back().m_Rotation,
		    m_Keyframes.back().m_Translation);
	}
	else
	{
		for (unsigned int i = 0; i < m_Keyframes.size() - 1; i++)
		{
			if (t > m_Keyframes[i].m_TimePosition && t < m_Keyframes[i + 1].m_TimePosition)
			{
				float timeSinceMostRecentKeyframe = t - m_Keyframes[i].m_TimePosition;
				float timeBetween = m_Keyframes[i + 1].m_TimePosition - m_Keyframes[i].m_TimePosition;
				float lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

				const Vector3& translation = Vector3::Lerp(
					m_Keyframes[i].m_Translation, 
					m_Keyframes[i + 1].m_Translation, 
					lerpFactor);
				
				const Quaternion& rotation = Quaternion::Slerp(
				    m_Keyframes[i].m_Rotation,
				    m_Keyframes[i + 1].m_Rotation,
				    lerpFactor);
				
				const Vector3& scale = Vector3::Lerp(
				    m_Keyframes[i].m_Scale,
				    m_Keyframes[i + 1].m_Scale,
				    lerpFactor);

				matrix = DirectX::XMMatrixAffineTransformation(
					scale,
					Vector4::Zero,
					rotation,
				    translation);

				// No need to check futher. This will be the only one needed.
				break;
			}
		}
	}
}

float BasicAnimation::getEndTime() const
{
	return m_Keyframes.back().m_TimePosition;
}

float BasicAnimation::getStartTime() const
{
	return m_Keyframes.front().m_TimePosition;
}
