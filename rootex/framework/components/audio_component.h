#pragma once

#include "audio/audio_source.h"
#include "component.h"
#include "entity.h"
#include "framework/components/transform_component.h"

class AudioComponent : public Component
{

public:
	TransformComponent* m_TransformComponent;

protected:
	bool m_IsPlayOnStart;
	bool m_IsAttenuated;
	ALfloat m_RolloffFactor;
	ALfloat m_ReferenceDistance;
	ALfloat m_MaxDistance;
	AudioSource::AttenuationModel m_AttenuationModel;

	AudioComponent(bool playOnStart, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	AudioComponent(AudioComponent&) = delete;
	~AudioComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioComponent;

	virtual bool setup() override;
	void update();

	virtual AudioSource* getAudioSource();
	virtual JSON::json getJSON() const;

	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	virtual String getName() const override { return "AudioComponent"; }
	ComponentID getComponentID() const { return s_ID; }
};