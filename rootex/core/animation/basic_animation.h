#pragma once

#include "common/common.h"

struct Keyframe
{
	float m_TimePosition;
	Vector3 m_Translation;
	Quaternion m_Rotation;
	Vector3 m_Scale;
};

struct BasicAnimation
{
	Vector<Keyframe> m_Keyframes;

	void interpolate(Matrix& matrix, float t);
	
	float getStartTime() const;
	float getEndTime() const;
};
