#pragma once

#include "audio/audio_source.h"
#include "component.h"
#include "entity.h"
#include "framework/components/transform_component.h"

class AudioComponent : public Component
{
	DEPENDS_ON(TransformComponent);

private:
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
	AudioComponent(bool playOnStart, bool isLooping, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	AudioComponent(AudioComponent&) = delete;
	~AudioComponent() = default;

	virtual bool setupData() override;

	void update();

	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	bool isAttenuated() { return m_IsAttenuated; }
	bool isLooping();

	void setLooping(bool enabled);
	void setAudioSource(AudioSource* audioSource) { m_AudioSource = audioSource; }
	AudioSource* getAudioSource() { return m_AudioSource; }

	virtual JSON::json getJSON() const;

	void draw() override;
};
