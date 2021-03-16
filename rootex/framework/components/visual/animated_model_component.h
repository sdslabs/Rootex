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
public:
	enum class AnimationMode : int
	{
		None = 0,
		Looping = 1,
		Alternating = 2
	};

private:
	static Component* Create(const JSON::json& componentData);

	friend class ECSFactory;
	friend class AnimationSystem;

protected:
	float m_TimeDirection = 1.0f;
	float m_TransitionTime = 1.0f;
	float m_RemainingTransitionTime = 0.0f;

	Ref<AnimatedModelResourceFile> m_AnimatedModelResourceFile;
	String m_CurrentAnimationName;
	float m_CurrentTimePosition;
	bool m_IsPlaying;
	bool m_IsPlayOnStart;
	AnimationMode m_AnimationMode;
	Vector<Matrix> m_FinalTransforms;

	AnimatedModelComponent(
	    bool isPlayOnStart,
	    Ref<AnimatedModelResourceFile> resFile,
	    const String& currentAnimationName,
	    AnimationMode mode,
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

	virtual Ref<AnimatedModelResourceFile> getAnimatedResourceFile() const { return m_AnimatedModelResourceFile; }
	virtual String getCurrentAnimationName() const { return m_CurrentAnimationName; }
	virtual float getCurrentTime() const { return m_CurrentTimePosition; }

	void update(float deltaMilliseconds);
	void setPlaying(bool enabled);
	void play();
	void stop();
	void setAnimation(const String& name);

	void transition(const String& name, float transitionTime);

	float getStartTime() const;
	float getEndTime() const;
	bool isPlaying() const { return m_IsPlaying; }
	bool hasEnded() const;

	void assignBoundingBox();
	void assignOverrides(Ref<AnimatedModelResourceFile> file, const HashMap<String, String>& materialOverrides);
	void setAnimatedResourceFile(Ref<AnimatedModelResourceFile> file, const HashMap<String, String>& materialOverrides);

	virtual const char* getName() const override { return "AnimatedModelComponent"; }
	ComponentID getComponentID() const override { return s_ID; }
	virtual JSON::json getJSON() const override;

	void draw() override;
};
