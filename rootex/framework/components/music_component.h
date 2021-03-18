#pragma once

#include "audio_component.h"

#include "core/audio/audio_source.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/event_manager.h"
#include "core/resource_loader.h"

class MusicComponent : public AudioComponent
{
	static Component* Create(const JSON::json& componentData);

	Ref<StreamingAudioSource> m_StreamingAudioSource;
	Ref<StreamingAudioBuffer> m_StreamingAudioBuffer;
	Ref<AudioResourceFile> m_AudioFile;

	MusicComponent(Ref<AudioResourceFile> audioFile, bool playOnStart, bool isLooping, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	virtual ~MusicComponent();

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::MusicComponent;

	virtual bool setupData() override;

	AudioResourceFile* getAudioFile() const { return m_AudioFile.get(); }
	void setAudioFile(Ref<AudioResourceFile> audioFile);

	virtual const char* getName() const override { return "MusicComponent"; };
	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual JSON::json getJSON() const override;

	void draw() override;
};
