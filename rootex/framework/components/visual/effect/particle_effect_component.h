#pragma once

#include "common/types.h"
#include "components/space/transform_component.h"
#include "resource_files/particle_effect_resource_file.h"

#include "Effekseer.h"

class ParticleEffectComponent : public Component
{
	DEFINE_COMPONENT(ParticleEffectComponent);
	DEPENDS_ON(TransformComponent);

	Effekseer::Handle m_EffectHandle;

	Ref<ParticleEffectResourceFile> m_EffectResource;
	int m_StartFrame;
	bool m_PlayOnStart;
	bool m_IsMoving;
	bool m_UseSpeed;
	float m_Speed;
	bool m_UseTarget;
	Vector3 m_TargetLocation;

public:
	ParticleEffectComponent(
	    Ref<ParticleEffectResourceFile> effect,
	    bool playOnStart,
	    bool isMoving,
	    int startFrame,
	    bool useSpeed,
	    float speed,
	    bool useTarget,
	    const Vector3& targetLocation);
	~ParticleEffectComponent() = default;

	bool isPlayOnStart() const { return m_PlayOnStart; }

	bool isMoving() const { return m_IsMoving; }
	void setMoving(bool enabled) { m_IsMoving = enabled; }

	bool isPaused() const;
	void setPlaying(bool enabled);
	void play();
	void stop();

	ParticleEffectResourceFile* getEffectResource() { return m_EffectResource.get(); }
	void setEffect(Ref<ParticleEffectResourceFile> effect);

	Effekseer::Handle getHandle() const { return m_EffectHandle; }

	JSON::json getJSON() const override;
	void draw() override;
};
