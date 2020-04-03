#pragma once

#include "component.h"
#include "audio/audio_source.h"

class AudioComponent : public Component
{
protected:
	bool m_IsPlayOnStart;

	AudioComponent(bool playOnStart);
	AudioComponent(AudioComponent&) = delete;
	~AudioComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioComponent;
	
	virtual AudioSource* getAudioSource() = 0;

	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	virtual String getName() const override { return "AudioComponent"; }
	ComponentID getComponentID() const { return s_ID; }
};
