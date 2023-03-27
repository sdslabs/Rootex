#pragma once

#include "audio/audio_source.h"
#include "component.h"
#include "entity.h"
#include "components/space/transform_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/physics/capsule_collider_component.h"

/// Component that plays audio according to the listener's position relative to the component.
class AudioComponent : public Component
{
	DEPENDS_ON(TransformComponent);
	SOFT_DEPENDS_ON(CapsuleColliderComponent);
	SOFT_DEPENDS_ON(BoxColliderComponent);
	SOFT_DEPENDS_ON(SphereColliderComponent);

	bool m_IsAttenuated;
	AudioSource::AttenuationModel m_AttenuationModel;
	ALfloat m_RolloffFactor;
	ALfloat m_ReferenceDistance;
	ALfloat m_MaxDistance;
	ALfloat m_Volume;
	AudioSource* m_AudioSource;

protected:
	bool m_IsPlayOnStart;
	bool m_IsLooping;

	String m_AttenuationModelName = "Linear";

	void setAudioSource(AudioSource* audioSource) { m_AudioSource = audioSource; }

public:
	AudioComponent(
	    Entity& owner,
	    bool playOnStart,
	    float volume,
	    bool isLooping,
	    bool attenuation,
	    AudioSource::AttenuationModel model,
	    ALfloat rolloffFactor,
	    ALfloat referenceDistance,
	    ALfloat maxDistance);
	virtual ~AudioComponent() = default;

	void update();

	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	bool isAttenuated() { return m_IsAttenuated; }

	void setPlaying(bool enabled);
	void play();
	void stop();
	// void getVolume(float ratio) { m_Volume=ratio*m_Volume; }

	void setLooping(bool enabled);
	bool isLooping();

	AudioSource* getAudioSource() { return m_AudioSource; }
	RigidBodyComponent* getCollider();

	bool setupData() override;
	JSON::json getJSON() const;
	void draw() override;
};
