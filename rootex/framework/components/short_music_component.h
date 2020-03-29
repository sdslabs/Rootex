#pragma once

#include "core/audio/static_audio_buffer.h"
#include "core/audio/audio_source.h"

#include "audio_component.h"

class ShortMusicComponent : public AudioComponent
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Ref<StaticAudioSource> m_StaticAudioSource;
	Ref<StaticAudioBuffer> m_StaticAudioBuffer;
	AudioResourceFile* m_AudioFile;

	ShortMusicComponent(AudioResourceFile* audioFile, bool playOnStart);
	virtual ~ShortMusicComponent();

	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::ShortMusicComponent;

	virtual bool setup() override;

	AudioSource* getAudioSource() override { return m_StaticAudioSource.get(); }
	AudioResourceFile* getAudioFile() const { return m_AudioFile; }
	virtual ComponentID getComponentID() const override { return s_ID; };
	virtual String getName() const override { return "ShortMusicComponent"; };
	virtual JSON::json getJSON() const override;

	void setAudioFile(AudioResourceFile* audioFile);

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif 
};
