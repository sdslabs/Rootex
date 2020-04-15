#pragma once

#include "audio_component.h"
#include "core/audio/audio_source.h"

class MusicComponent : public AudioComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Ref<StreamingAudioSource> m_StreamingAudioSource;
	Ref<StreamingAudioBuffer> m_StreamingAudioBuffer;
	AudioResourceFile* m_AudioFile;

	MusicComponent(AudioResourceFile* audioFile, bool playOnStart, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	virtual ~MusicComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::MusicComponent;

	virtual bool setup() override;

	AudioSource* getAudioSource() override { return m_StreamingAudioSource.get(); }
	AudioResourceFile* getAudioFile() const { return m_AudioFile; }
	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual String getName() const override { return "MusicComponent"; };
	virtual JSON::json getJSON() const override;

	void setAudioFile(AudioResourceFile* audioFile);

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
