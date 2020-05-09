#pragma once

#include "component.h"

class TransformComponent;

class TransformAnimationComponent : public Component
{
public:
	struct Keyframe
	{
		float m_TimePosition;
		Vector3 m_Translation;
		Quaternion m_Rotation;
		Vector3 m_Scale;
	};

private:
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

	Vector<Keyframe> m_Keyframes;
	bool m_IsPlayOnStart;
	bool m_IsLooping;

	bool m_IsPlaying;
	float m_CurrentTimePosition;

	TransformComponent* m_TransformComponent;
	Vector3 m_InitialPosition;
	Quaternion m_InitialRotation;
	Vector3 m_InitialScale;

	TransformAnimationComponent(const Vector<Keyframe> keyframes, bool isPlayOnStart, bool isLooping);
	TransformAnimationComponent(TransformComponent&) = delete;
	~TransformAnimationComponent() = default;

	friend class TransformAnimationSystem;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformAnimationComponent;

	virtual bool setup() override;

	void pushKeyframe(float timePosition, const Vector3& position, const Quaternion& rotation, const Vector3& scale);
	void popKeyframe(int count);
	bool isPlaying() const { return !hasEnded(); }
	bool isLooping() const { return m_IsLooping; }
	bool hasEnded() const;
	float getStartTime() const;
	float getEndTime() const;

	void interpolate(float t);

	virtual String getName() const override { return "TransformAnimationComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw() override;
#endif // ROOTEX_EDITOR
};
