#pragma once

#include "audio/audio_source.h"
#include "component.h"
#include "entity.h"
#include "components/space/transform_component.h"

class AudioComponent : public Component
{
	DEPENDS_ON(TransformComponent);

	bool m_IsAttenuated;
	AudioSource::AttenuationModel m_AttenuationModel;
	ALfloat m_RolloffFactor;
	ALfloat m_ReferenceDistance;
	ALfloat m_MaxDistance;
	AudioSource* m_AudioSource;

protected:
	bool m_IsPlayOnStart;
	bool m_IsLooping;

	String m_AttenuationModelName = "Linear";

public:
	AudioComponent(
	    bool playOnStart,
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

	void setLooping(bool enabled);
	bool isLooping();

	void setAudioSource(AudioSource* audioSource) { m_AudioSource = audioSource; }
	AudioSource* getAudioSource() { return m_AudioSource; }

	bool setupData() override;
	JSON::json getJSON() const;
	void draw() override;
};
