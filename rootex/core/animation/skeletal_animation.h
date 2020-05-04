#pragma once

#include "common/common.h"
#include "core/animation/basic_animation.h"

struct SkeletalAnimation
{
	Vector<BasicAnimation> m_BoneAnimations;

	void interpolate(Vector<Matrix>& animation, float t) const;

	float getStartTime() const;
	float getEndTime() const;
};
