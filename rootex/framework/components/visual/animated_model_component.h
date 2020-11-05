#pragma once

#include "framework/component.h"
#include "core/resource_file.h"
#include "framework/entity_factory.h"
#include "renderable_component.h"
#include "framework/components/transform_component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/systems/animation_system.h"

class AnimatedModelComponent : public RenderableComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();
	
	friend class EntityFactory;
	friend class AnimationSystem;

protected:
	AnimatedModelResourceFile* m_AnimatedModelResourceFile;
	
	String m_CurrentAnimationName;
	float m_CurrentTimePosition;
	bool m_IsPlaying;
	Vector<Matrix> m_FinalTransforms;

	AnimatedModelComponent(unsigned int renderPass, const HashMap<String, String>& materialOverrides, AnimatedModelResourceFile* resFile, bool isVisible, const Vector<EntityID>& affectingStaticLightEntityIDs);
	AnimatedModelComponent(AnimatedModelComponent&) = delete;
	virtual ~AnimatedModelComponent() = default;

public:
	static void RegisterAPI(sol::table& rootex);
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AnimatedModelComponent;

	virtual void render();

	virtual AnimatedModelResourceFile* getAnimatedResourceFile() const { return m_AnimatedModelResourceFile; }
	virtual String getCurrentAnimationName() const { return m_CurrentAnimationName; }
	virtual float getCurrentTime() const { return m_CurrentTimePosition; }

	void setAnimatedResourceFile(AnimatedModelResourceFile* file, const HashMap<String, String>& materialOverrides);

	virtual String getName() const override { return "AnimatedModelComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif //ROOTEX_EDITOR
};
