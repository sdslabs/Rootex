#pragma once

#include "audio_component.h"

#include "core/audio/audio_source.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/event_manager.h"
#include "core/resource_loader.h"

class MusicComponent : public AudioComponent
{
	DEFINE_COMPONENT(MusicComponent);

	Ref<StreamingAudioSource> m_StreamingAudioSource;
	Ref<StreamingAudioBuffer> m_StreamingAudioBuffer;
	Ref<AudioResourceFile> m_AudioFile;

public:
	MusicComponent(
	    Ref<AudioResourceFile> audioFile,
	    bool playOnStart,
	    bool isLooping,
	    bool attenuation,
	    AudioSource::AttenuationModel model,
	    ALfloat rolloffFactor,
	    ALfloat referenceDistance,
	    ALfloat maxDistance);
	~MusicComponent();

	AudioResourceFile* getAudioFile() const { return m_AudioFile.get(); }
	void setAudioFile(Ref<AudioResourceFile> audioFile);

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};
