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

/// Represents a node in the skeleton heirarchy(a tree like structure) of an animated model.
struct SkeletonNode
{
	Vector<Ptr<SkeletonNode>> m_Children;
	String m_Name;
	Matrix m_LocalBindTransform;
};

/// Stores the animation keyframes for a bone.
class BoneAnimation
{
	Vector<TranslationKeyframe> m_Translation;
	Vector<RotationKeyframe> m_Rotation;
	Vector<ScalingKeyframe> m_Scaling;

	float getEndTime() const;
	float getStartTime() const;

public:
	BoneAnimation() = default;
	BoneAnimation(const BoneAnimation&) = default;
	~BoneAnimation() = default;

	void addTranslationKeyframe(TranslationKeyframe& keyframe) { m_Translation.push_back(keyframe); }
	void addRotationKeyframe(RotationKeyframe& keyframe) { m_Rotation.push_back(keyframe); }
	void addScalingKeyframe(ScalingKeyframe& keyframe) { m_Scaling.push_back(keyframe); }

	/// Lineary interpolates the two nearest keyframes to the current time.
	Matrix interpolate(float time);
};

/// Represents a single animation of a model with its bone animation data.
class SkeletalAnimation
{
	float m_Duration;
	HashMap<String, BoneAnimation> m_BoneAnimations;

public:
	SkeletalAnimation() = default;
	SkeletalAnimation(const SkeletalAnimation&) = default;
	~SkeletalAnimation() = default;

	/// Calls the interpolate() function of each BoneAnimation
	Matrix interpolate(const String& nodeName, float currentTime);

	float getStartTime() const;
	float getEndTime() const;

	void setDuration(float time) { m_Duration = time; }
	void addBoneAnimation(String boneName, BoneAnimation& boneAnimation) { m_BoneAnimations[boneName] = boneAnimation; }
};
