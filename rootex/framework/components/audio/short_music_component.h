#pragma once

#include "audio_component.h"

#include "core/audio/audio_source.h"
#include "core/audio/static_audio_buffer.h"
#include "core/event_manager.h"
#include "core/resource_loader.h"

class ShortMusicComponent : public AudioComponent
{
	DEFINE_COMPONENT(ShortMusicComponent);

	Ref<StaticAudioSource> m_StaticAudioSource;
	Ref<StaticAudioBuffer> m_StaticAudioBuffer;
	Ref<AudioResourceFile> m_AudioFile;

public:
	ShortMusicComponent(
	    Ref<AudioResourceFile> audioFile,
	    bool playOnStart,
	    bool isLooping,
	    bool attenuation,
	    AudioSource::AttenuationModel model,
	    ALfloat rolloffFactor,
	    ALfloat referenceDistance,
	    ALfloat maxDistance);
	~ShortMusicComponent();

	AudioResourceFile* getAudioFile() const { return m_AudioFile.get(); }
	void setAudioFile(Ref<AudioResourceFile> audioFile);

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};
