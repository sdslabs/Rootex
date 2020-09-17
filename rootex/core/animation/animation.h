#pragma once

#include "common/common.h"

struct TranslationKeyframe
{
	float m_Time;
	Vector3 m_Translation;
};

struct RotationKeyframe 
{
	float m_Time;
	Quaternion m_Rotation;
};

struct ScalingKeyframe
{
	float m_Time;
	Vector3 m_Scaling;
};

struct BoneAnimation
{
	Vector<TranslationKeyframe> m_Translation;
	Vector<RotationKeyframe> m_Rotation;
	Vector<ScalingKeyframe> m_Scaling;

public:
	void interpolate(float time, Matrix& bonetransform);
	float getStartTime() const;
	float getEndTime() const;
};

struct SkeletalAnimation
{
	float m_Duration;
	Vector<BoneAnimation> m_BoneAnimations;

public:
	void interpolate(float time, Vector<Matrix>& transforms);
	float getEndTime() const;
};
