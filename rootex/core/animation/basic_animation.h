#pragma once

#include "common/common.h"

struct TranslationKeyframe
{
	float m_TimePosition;
	Vector3 m_Translation;
};

struct RotationKeyframe
{
	float m_TimePosition;
	Quaternion m_Rotation;
};

struct ScaleKeyframe
{
	float m_TimePosition;
	Vector3 m_Scale;
};

struct BasicAnimation
{
	Vector<TranslationKeyframe> m_TranslationKeyframes;
	Vector<RotationKeyframe> m_RotationKeyframes;
	Vector<ScaleKeyframe> m_ScaleKeyframes;

	void interpolate(Matrix& matrix, float t) const;
	
	void addKeyframe(const TranslationKeyframe& key) { m_TranslationKeyframes.push_back(key); }
	void addKeyframe(const RotationKeyframe& key) { m_RotationKeyframes.push_back(key); }
	void addKeyframe(const ScaleKeyframe& key) { m_ScaleKeyframes.push_back(key); }

	float getStartTime() const;
	float getEndTime() const;
};
