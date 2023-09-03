#pragma once

#include "audio_component.h"

#include "core/audio/audio_source.h"
#include "core/audio/streaming_audio_buffer.h"
#include "core/event_manager.h"
#include "core/resource_loader.h"

class MusicComponent : public AudioComponent
{
	COMPONENT(MusicComponent, Category::Audio);

	ALfloat volume_comp;
	Ref<StreamingAudioSource> m_StreamingAudioSource;
	Ref<StreamingAudioBuffer> m_StreamingAudioBuffer;
	Ref<AudioResourceFile> m_AudioFile;

public:
	MusicComponent(Entity& owner, const JSON::json& data);
	~MusicComponent();
	static Map<ALuint, String> music_comp_audio_bus;
	static Map<ALuint,ALfloat > music_comp_volume;
	AudioResourceFile* getAudioFile() const { return m_AudioFile.get(); }
	void to_json(JSON::json& j, const MusicComponent& a);
	void from_json(const JSON::json& j, MusicComponent& a);
	void setAudioFile(Ref<AudioResourceFile> audioFile);
	// void updateVolume(float ratio);

	bool setupData() override;
	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(MusicComponent);
