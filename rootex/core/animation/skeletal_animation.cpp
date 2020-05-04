#include "skeletal_animation.h"

void SkeletalAnimation::interpolate(Vector<Matrix>& animation, float t) const
{
	for (int i = 0; i < m_BoneAnimations.size(); i++)
	{
		m_BoneAnimations[i].interpolate(animation[i], t);
	}
}

float SkeletalAnimation::getStartTime() const
{
	return m_BoneAnimations.front().getStartTime();
}

float SkeletalAnimation::getEndTime() const
{
	return m_BoneAnimations.back().getStartTime();
}
