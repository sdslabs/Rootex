#pragma once

#include "framework/component.h"
#include "model_component.h"
#include "core/resource_file.h"
#include "core/resource_files/animated_model_resource_file.h"
#include "renderable_component.h"
#include "framework/components/transform_component.h"
#include "framework/systems/animation_system.h"
#include "scene.h"

class AnimatedModelComponent : public RenderableComponent
{
	static Component* Create(const JSON::json& componentData);

	friend class ECSFactory;
	friend class AnimationSystem;

protected:
	AnimatedModelResourceFile* m_AnimatedModelResourceFile;

	String m_CurrentAnimationName;
	float m_CurrentTimePosition;
	bool m_IsPlaying;
	bool m_IsPlayOnStart;
	Vector<Matrix> m_FinalTransforms;

	AnimatedModelComponent(
	    bool isPlayOnStart,
	    AnimatedModelResourceFile* resFile,
	    const String& currentAnimationName,
	    unsigned int renderPass,
	    const HashMap<String, String>& materialOverrides,
	    bool isVisible,
	    bool lodEnable,
	    float lodBias,
	    float lodDistance,
	    const Vector<SceneID>& affectingStaticLightIDs);
	AnimatedModelComponent(AnimatedModelComponent&) = delete;
	virtual ~AnimatedModelComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AnimatedModelComponent;

	virtual bool setupData() override;
	virtual bool preRender(float deltaMilliseconds) override;
	virtual void render(float viewDistance) override;

	virtual AnimatedModelResourceFile* getAnimatedResourceFile() const { return m_AnimatedModelResourceFile; }
	virtual String getCurrentAnimationName() const { return m_CurrentAnimationName; }
	virtual float getCurrentTime() const { return m_CurrentTimePosition; }

	void assignBoundingBox();
	void assignOverrides(AnimatedModelResourceFile* file, const HashMap<String, String>& materialOverrides);
	void setAnimatedResourceFile(AnimatedModelResourceFile* file, const HashMap<String, String>& materialOverrides);

	virtual const char* getName() const override { return "AnimatedModelComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

	void draw() override;
};
