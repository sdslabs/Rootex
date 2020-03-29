#pragma once

#include "component.h"
#include "core/audio/audio_source.h"

class MusicComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	Ref<StreamingAudioSource> m_StreamingAudioSource;
	Ref<StreamingAudioBuffer> m_StreamingAudioBuffer;
	AudioResourceFile* m_AudioFile;

	MusicComponent(AudioResourceFile* audioFile);
	virtual ~MusicComponent();
	
	friend class EntityFactory;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::MusicComponent;

	virtual bool setup() override;

	StreamingAudioSource* getAudioSource() const { return m_StreamingAudioSource.get(); }
	AudioResourceFile* getAudioFile() const { return m_AudioFile; }
	virtual ComponentID getComponentID() const { return s_ID; };
	virtual String getName() const { return "MusicComponent"; };
	virtual JSON::json getJSON() const;

	void setAudioFile(AudioResourceFile* audioFile);

#ifdef ROOTEX_EDITOR
	virtual void draw();
#endif
};
