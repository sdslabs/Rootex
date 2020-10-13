#include "animation.h"

Matrix& SkeletalAnimation::interpolate(const String& nodeName, float currentTime)
{
	if (m_BoneAnimations.find(nodeName) != m_BoneAnimations.end())
	{
		return m_BoneAnimations[nodeName].interpolate(currentTime);
	}
	else
	{
		return Matrix(Matrix::Identity);
	}
}

float SkeletalAnimation::getEndTime() const
{
	return m_Duration;
}

Matrix& BoneAnimation::interpolate(float time)
{
	if (time <= getStartTime())
	{
		Matrix transform = DirectX::XMMatrixAffineTransformation(
			m_Scaling.front().m_Scaling,
			Vector3::Zero,
			m_Rotation.front().m_Rotation,
			m_Translation.front().m_Translation
		);
		return transform;
	}

	else if (time >= getEndTime())
	{
		Matrix transform = DirectX::XMMatrixAffineTransformation(
			m_Scaling.back().m_Scaling,
			Vector3::Zero,
			m_Rotation.back().m_Rotation,
			m_Translation.back().m_Translation
		);
		return transform;
	}

	else
	{
		unsigned int index = m_Scaling.size();
		for (unsigned int i = 1; i < m_Scaling.size(); i++)
		{
			if (m_Scaling[i].m_Time >= time)
			{
				index = i;
				break;
			}
		}

		Vector3 scaling = Vector3::Zero;
		float timeSinceMostRecentKeyframe = time - m_Scaling[index - 1].m_Time;
		float timeBetween = m_Scaling[index].m_Time - m_Scaling[index].m_Time;
		float lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

		scaling = Vector3::Lerp(
			m_Scaling[index - 1].m_Scaling,
			m_Scaling[index].m_Scaling,
			lerpFactor);

		index = m_Rotation.size();
		for (unsigned int i = 1; i < m_Rotation.size(); i++)
		{
			if (m_Rotation[i].m_Time >= time)
			{
				index = i;
				break;
			}
		}

		Quaternion rotation = Quaternion::Identity;
		timeSinceMostRecentKeyframe = time - m_Rotation[index - 1].m_Time;
		timeBetween = m_Rotation[index].m_Time - m_Scaling[index].m_Time;
		lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

		rotation = Quaternion::Slerp(
		    m_Rotation[index - 1].m_Rotation,
		    m_Rotation[index].m_Rotation,
		    lerpFactor);
		rotation.Normalize();

		index = m_Translation.size();
		for (unsigned int i = 1; i < m_Translation.size(); i++)
		{
			index = i;
			break;
		}

		Vector3 translation = Vector3::Zero;
		timeSinceMostRecentKeyframe = time - m_Translation[index - 1].m_Time;
		timeBetween = m_Translation[index].m_Time - m_Translation[index - 1].m_Time;
		lerpFactor = timeSinceMostRecentKeyframe / timeBetween;

		translation = Vector3::Lerp(
		    m_Translation[index - 1].m_Translation,
		    m_Translation[index].m_Translation,
		    lerpFactor);

		Matrix transform = DirectX::XMMatrixAffineTransformation(
		    scaling,
		    Vector3::Zero,
		    rotation,
		    translation
		);
		return transform;
	}
}

float BoneAnimation::getStartTime() const
{
	return min(m_Translation.front().m_Time, min(m_Rotation.front().m_Time, m_Scaling.front().m_Time));
}

float BoneAnimation::getEndTime() const
{
	return max(m_Translation.back().m_Time, max(m_Rotation.back().m_Time, m_Scaling.back().m_Time));
}
