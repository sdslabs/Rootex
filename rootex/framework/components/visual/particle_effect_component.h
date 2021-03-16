#pragma once

#include "common/types.h"
#include "components/transform_component.h"
#include "resource_files/particle_effect_resource_file.h"

#include "Effekseer.h"

class ParticleEffectComponent : public Component
{
	DEPENDS_ON(TransformComponent);

	static Component* Create(const JSON::json& componentData);

	Effekseer::Handle m_EffectHandle;

	ParticleEffectResourceFile* m_EffectResource;
	int m_StartFrame;
	bool m_PlayOnStart;
	bool m_IsMoving;
	bool m_UseSpeed;
	float m_Speed;
	bool m_UseTarget;
	Vector3 m_TargetLocation;

	friend class ECSFactory;

	ParticleEffectComponent(
	    ParticleEffectResourceFile* effect,
	    bool playOnStart,
	    bool isMoving,
	    int startFrame,
	    bool useSpeed,
	    float speed,
	    bool useTarget,
	    const Vector3& targetLocation);
	ParticleEffectComponent(const ParticleEffectComponent&) = delete;
	~ParticleEffectComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ParticleEffectComponent;

	bool isPlayOnStart() const { return m_PlayOnStart; }

	bool isMoving() const { return m_IsMoving; }
	void setMoving(bool enabled) { m_IsMoving = enabled; }

	bool isPaused() const;
	void setPlaying(bool enabled);
	void play();
	void stop();

	ParticleEffectResourceFile* getEffectResource() { return m_EffectResource; }
	void setEffect(ParticleEffectResourceFile* effect);

	Effekseer::Handle getHandle() const { return m_EffectHandle; }

	virtual const char* getName() const override { return "ParticleEffectComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const override;

	virtual void draw() override;
};
