#pragma once

#include "entity.h"
#include "component.h"
#include "transform_component.h"
#include "core/animation/basic_animation.h"

class TransformAnimationComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

	BasicAnimation m_BasicAnimation;
	float m_CurrentTimePosition;
	float m_Duration;
	bool m_IsPlayOnStart;
	bool m_IsPlaying;
	bool m_IsLooping;

	TransformComponent* m_TransformComponent;
	Vector3 m_InitialPosition;
	Quaternion m_InitialRotation;
	Vector3 m_InitialScale;

	TransformAnimationComponent(bool isPlayOnStart, bool isLooping, const Vector<Keyframe>& keyframes);
	TransformAnimationComponent(TransformAnimationComponent&) = delete;
	~TransformAnimationComponent() = default;

	friend class AnimationSystem;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::TransformAnimationComponent;

	bool isPlaying() const { return m_IsPlaying && m_CurrentTimePosition <= m_BasicAnimation.getEndTime(); }
	bool isLooping() const { return m_IsLooping; }

	void play();
	void pause();
	void reset();

	virtual bool setup() override;

	virtual String getName() const override { return "TransformAnimationComponent"; }
	virtual ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif // ROOTEX_EDITOR
};
