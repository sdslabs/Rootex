#pragma once

#include "audio_component.h"

#include "core/audio/audio_source.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/event_manager.h"
#include "core/resource_loader.h"

class MusicComponent : public AudioComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Ref<StreamingAudioSource> m_StreamingAudioSource;
	Ref<StreamingAudioBuffer> m_StreamingAudioBuffer;
	AudioResourceFile* m_AudioFile;

	MusicComponent(AudioResourceFile* audioFile, bool playOnStart, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	virtual ~MusicComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::MusicComponent;

	virtual bool setup() override;

	AudioResourceFile* getAudioFile() const { return m_AudioFile; }
	void setAudioFile(AudioResourceFile* audioFile);

	virtual String getName() const override { return "MusicComponent"; };
	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
