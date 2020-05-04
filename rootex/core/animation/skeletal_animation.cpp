#include "skeletal_animation.h"

void SkeletalAnimation::addBoneAnimation(const BasicAnimation& boneAnimation)
{
	m_BoneAnimations.push_back(boneAnimation);
}
