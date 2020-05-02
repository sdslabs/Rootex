#pragma once

#include "visual_component.h"
#include "core/animation/bone_animation.h"

class AnimatedCharacterComponent : public VisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

	float m_CurrentTimePosition;
	BoneAnimation m_BoneAnimation;

	AnimatedCharacterComponent();
	AnimatedCharacterComponent(AnimatedCharacterComponent&) = delete;
	~AnimatedCharacterComponent() = default;

	friend class AnimationSystem;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AnimatedCharacterVisualComponent;

	virtual String getName() const override { return "AnimatedCharacterComponent"; }
	ComponentID getComponentID() const override { return s_ID; }

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif // ROOTEX_EDITOR
};
