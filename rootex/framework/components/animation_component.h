#pragma once

#include "component.h"

#include "core/resource_file.h"
#include "core/animation/skeletal_animation.h"

class AnimationComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

	Map<String, SkeletalAnimation> m_Animations;

	AnimationComponent(SkeletalAnimationResourceFile* animationFile);
	AnimationComponent(AnimationComponent&) = delete;
	~AnimationComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AnimationComponent;

	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual String getName() const override { return "AnimationComponent"; };

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
