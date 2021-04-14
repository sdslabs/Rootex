#pragma once

#include "framework/component.h"
#include "model_component.h"
#include "core/resource_file.h"
#include "core/resource_files/animated_model_resource_file.h"
#include "renderable_component.h"
#include "components/space/transform_component.h"
#include "systems/animation_system.h"

class AnimatedModelComponent : public RenderableComponent
{
	COMPONENT(AnimatedModelComponent, Category::Model);

public:
	enum class AnimationMode : int
	{
		None = 0,
		Looping = 1,
		Alternating = 2
	};

protected:
	float m_TimeDirection = 1.0f;
	float m_TransitionTime = 1.0f;
	float m_RemainingTransitionTime = 0.0f;

	Ref<AnimatedModelResourceFile> m_AnimatedModelResourceFile;
	String m_CurrentAnimationName;
	float m_CurrentTimePosition;
	float m_SpeedMultiplier;
	RootExclusion m_RootExclusion;
	bool m_IsPlaying;
	bool m_IsPlayOnStart;
	AnimationMode m_AnimationMode;
	Vector<Matrix> m_FinalTransforms;

public:
	AnimatedModelComponent(Entity& owner, const JSON::json& data);
	~AnimatedModelComponent() = default;

	bool preRender(float deltaMilliseconds) override;
	void render(float viewDistance) override;

	String getCurrentAnimationName() const { return m_CurrentAnimationName; }
	float getCurrentTime() const { return m_CurrentTimePosition; }

	void checkCurrentAnimationExists();

	void update(float deltaMilliseconds);

	void setPlaying(bool enabled);
	void play();
	void stop();

	void setAnimation(const String& name);
	void transition(const String& name, float transitionTime);

	void setSpeedMultiplier(float speedMul) { m_SpeedMultiplier = speedMul; }

	float getStartTime() const;
	float getEndTime() const;

	bool isPlaying() const { return m_IsPlaying; }
	bool hasEnded() const;

	void assignBoundingBox();
	void assignOverrides(Ref<AnimatedModelResourceFile> file, const HashMap<String, String>& materialOverrides);

	void setAnimatedResourceFile(Ref<AnimatedModelResourceFile> file, const HashMap<String, String>& materialOverrides);
	Ref<AnimatedModelResourceFile> getAnimatedResourceFile() const { return m_AnimatedModelResourceFile; }

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(AnimatedModelComponent);
