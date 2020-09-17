#pragma once

#include "framework/component.h"
#include "core/resource_file.h"
#include "framework/entity_factory.h"
#include "framework/components/transform_component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/systems/animation_system.h"

class AnimatedModelComponent : public Component
{
	static AnimatedModelComponent* Create(const JSON::json& componentData);
	static AnimatedModelComponent* CreateDefault();
	
	friend class EntityFactory;
	friend class AnimationSystem;

protected:
	AnimatedModelResourceFile* m_AnimatedModelResourceFile;
	bool m_IsVisible;
	UINT m_RenderPass;
	String m_CurrentAnimationName;
	float m_CurrentTimePosition;
	Vector<Matrix> m_FinalTransforms;

	TransformComponent* m_TransformComponent;
	HierarchyComponent* m_HierarchyComponent;

	AnimatedModelComponent(UINT renderPass, AnimatedModelResourceFile* resFile, bool isVisible);
	AnimatedModelComponent(AnimatedModelComponent&) = delete;
	virtual ~AnimatedModelComponent() = default;

public:
	virtual void RegisterAPI(sol::state& rootex);
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AnimatedModelComponent;

	virtual bool setup() override;

	virtual bool preRender();
	virtual void render();
	virtual void postRender();

	virtual AnimatedModelResourceFile* getAnimatedResourceFile() const { return m_AnimatedModelResourceFile; }
	virtual String getCurrentAnimationName() const { return m_CurrentAnimationName; }
	virtual float getCurrentTime() const { return m_CurrentTimePosition; }

	void setAnimatedResourceFile(AnimatedModelResourceFile* file) { m_AnimatedModelResourceFile = file; }

	virtual String getName() const override { return "AnimatedModelComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif //ROOTEX_EDITOR
};
