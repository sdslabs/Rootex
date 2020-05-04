#pragma once

#include "common/common.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/index_buffer.h"
#include "core/animation/basic_animation.h"

struct Skeleton
{
	Bone m_Root;
	Vector<VertexData> m_Vertices;
	Vector<float> m_Indices;
};

struct BoneWeight
{
	unsigned int m_VertexID;
	float m_Weight;
};

struct Bone
{
	Bone* m_BoneParent;
	Vector<Bone> m_BoneChildren;
	Matrix m_BoneOffset;
	String m_BoneName;
	BasicAnimation m_BoneAnimation;
	Vector<BoneWeight> m_AffectedVertices;
};

struct SkeletalAnimation
{
	Skeleton m_Skeleton;

	void interpolate(Vector<Matrix>& animation, float t) const;

	void addBoneAnimation(const BasicAnimation& boneAnimation);

	float getStartTime() const;
	float getEndTime() const;
};
