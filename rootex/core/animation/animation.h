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
};

struct SkeletalAnimation
{
	float m_Duration;
	Vector<BoneAnimation> m_BoneAnimations;
};