#pragma once

#include "audio_component.h"

#include "core/audio/audio_source.h"
#include "core/audio/static_audio_buffer.h"
#include "core/event_manager.h"
#include "core/resource_loader.h"

class ShortMusicComponent : public AudioComponent
{
	static Component* Create(const JSON::json& componentData);

	Ref<StaticAudioSource> m_StaticAudioSource;
	Ref<StaticAudioBuffer> m_StaticAudioBuffer;
	Ref<AudioResourceFile> m_AudioFile;

	ShortMusicComponent(Ref<AudioResourceFile> audioFile, bool playOnStart, bool isLooping, bool attenuation, AudioSource::AttenuationModel model, ALfloat rolloffFactor, ALfloat referenceDistance, ALfloat maxDistance);
	virtual ~ShortMusicComponent();

	friend class ECSFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ShortMusicComponent;

	virtual bool setupData() override;

	AudioResourceFile* getAudioFile() const { return m_AudioFile.get(); }
	void setAudioFile(Ref<AudioResourceFile> audioFile);

	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual const char* getName() const override { return "ShortMusicComponent"; };
	virtual JSON::json getJSON() const override;

	void draw() override;
};
