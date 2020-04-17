#pragma once

#include "audio_component.h"

#include "core/audio/audio_source.h"
#include "core/audio/static_audio_buffer.h"
#include "core/event_manager.h"
#include "core/resource_loader.h"

class ShortMusicComponent : public AudioComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Ref<StaticAudioSource> m_StaticAudioSource;
	Ref<StaticAudioBuffer> m_StaticAudioBuffer;
	AudioResourceFile* m_AudioFile;

	ShortMusicComponent(AudioResourceFile* audioFile, bool playOnStart, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	virtual ~ShortMusicComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ShortMusicComponent;

	virtual bool setup() override;

	//AudioSource* getAudioSource() override { return m_StaticAudioSource.get(); }
	AudioResourceFile* getAudioFile() const { return m_AudioFile; }

	void setAudioFile(AudioResourceFile* audioFile);

	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual String getName() const override { return "ShortMusicComponent"; };
	virtual JSON::json getJSON() const override;

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
