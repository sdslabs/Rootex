#pragma once

#include "component.h"
#include "components/transform_component.h"

class TransformAnimationComponent : public Component
{
	DEPENDS_ON(TransformComponent);

public:
	struct Keyframe
	{
		float timePosition;
		Matrix transform;
	};

	/// First word is for incoming, second word for outgoing
	enum class TransitionType
	{
		SmashSmash = 0,
		EaseEase = 1,
		SmashEase = 2,
		EaseSmash = 3
	};

	enum class AnimationMode
	{
		None = 0,
		Looping = 1,
		Alternating = 2
	};

private:
	static Component* Create(const JSON::json& componentData);

	friend class EntityFactory;

	Vector<Keyframe> m_Keyframes;
	bool m_IsPlayOnStart;
	AnimationMode m_AnimationMode;
	TransitionType m_TransitionType;

	bool m_IsPlaying;
	float m_CurrentTimePosition;
	float m_TimeDirection;

	TransformAnimationComponent(const Vector<Keyframe> keyframes, bool isPlayOnStart, AnimationMode animationMode, TransitionType transition);
	TransformAnimationComponent(TransformComponent&) = delete;
	~TransformAnimationComponent() = default;

	Matrix interpolateMatrix(const Matrix& left, const Matrix& right, float lerpFactor);

	friend class TransformAnimationSystem;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformAnimationComponent;

	virtual bool setupData() override;

	void pushKeyframe(const Keyframe& keyFrame);
	void popKeyframe(int count);
	bool isPlaying() const { return m_IsPlaying; }
	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	bool hasEnded() const;
	float getStartTime() const;
	float getEndTime() const;
	void reset();

	void interpolate(float t);

	void setPlaying(bool enabled);

	virtual void onTrigger() override;
	virtual const char* getName() const override { return "TransformAnimationComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
