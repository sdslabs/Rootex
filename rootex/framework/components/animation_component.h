#pragma once

#include "component.h"

#include "core/resource_file.h"
#include "core/animation/skeletal_animation.h"

#include "framework/components/visual/visual_component.h"

class AnimationComponent : public VisualComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

	SkeletalAnimationResourceFile* m_AnimationFile;
	float m_CurrentTimePosition;
	String m_CurrentClipName;
	Vector<Matrix> m_FinalTransforms;
	Ref<Material> m_Material;

	AnimationComponent(RenderPass renderPass, bool isVisible, SkeletalAnimationResourceFile* animationFile);
	AnimationComponent(AnimationComponent&) = delete;
	~AnimationComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AnimationComponent;

	virtual bool preRender() { return true; }
	virtual bool isVisible() const { return m_IsVisible; }
	virtual void render(RenderPass renderPass) {}
	virtual void renderChildren(RenderPass renderPass) {}
	virtual void postRender() {}

	void setAnimationFile(SkeletalAnimationResourceFile* file) { m_AnimationFile = file; }

	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual String getName() const override { return "AnimationComponent"; };
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
