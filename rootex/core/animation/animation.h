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
	Matrix& interpolate(float time);
	float getStartTime() const;
	float getEndTime() const;
};

struct SkeletonNode
{
	Vector<SkeletonNode*> m_Children;
	String m_Name;
	unsigned int m_Index;
	Matrix m_LocalBindTransform;
};

struct SkeletalAnimation
{
	float m_Duration;
	HashMap<String, BoneAnimation> m_BoneAnimations;

public:
	Matrix& interpolate(const String& nodeName, float currentTime);
	float getEndTime() const;
};
