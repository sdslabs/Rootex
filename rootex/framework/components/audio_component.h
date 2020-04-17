#pragma once

#include "audio/audio_source.h"
#include "component.h"
#include "entity.h"
#include "framework/components/transform_component.h"

class AudioComponent : public Component
{
private:
	bool m_IsAttenuated;
	AudioSource::AttenuationModel m_AttenuationModel;
	ALfloat m_RolloffFactor;
	ALfloat m_ReferenceDistance;
	ALfloat m_MaxDistance;
	AudioSource* m_AudioSource;

protected:
	bool m_IsPlayOnStart;
	TransformComponent* m_TransformComponent;

#ifdef ROOTEX_EDITOR
	String m_AttenuationModelName = "Linear";
#endif // ROOTEX_EDITOR

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioComponent;

	AudioComponent(bool playOnStart, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	AudioComponent(AudioComponent&) = delete;
	~AudioComponent() = default;

	virtual bool setup();

	void update();

	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	bool isAttenuated() { return m_IsAttenuated; }
	//ALfloat getMaxDistance() { return m_MaxDistance; }
	//ALfloat getRolloffFactor() { return m_RolloffFactor; }
	//ALfloat getReferenceDistance() { return m_ReferenceDistance; }
	//AudioSource::AttenuationModel getAttenuationModel() { return m_AttenuationModel; }

	//void setMaxDistance(ALfloat maxDistance) { m_MaxDistance = maxDistance; }
	//void setRolloffFactor(ALfloat rolloffFactor) { m_RolloffFactor = rolloffFactor; }
	//void setReferenceDistance(ALfloat referenceDistance) { m_ReferenceDistance = referenceDistance; }
	//void setAttenuationModel(AudioSource::AttenuationModel model) { m_AttenuationModel = model; }

	void setAudioSource(AudioSource* audioSource) { m_AudioSource = audioSource; }
	AudioSource* getAudioSource() { return m_AudioSource; }

	virtual String getName() const override { return "AudioComponent"; }
	ComponentID getComponentID() const { return s_ID; }
	virtual JSON::json getJSON() const;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};