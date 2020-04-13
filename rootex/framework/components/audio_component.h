#pragma once

#include "audio/audio_source.h"
#include "component.h"
#include "entity.h";
#include "framework/components/transform_component.h"

class AudioComponent : public Component
{

private:
	TransformComponent* m_TransformComponent;

protected:
	bool m_IsPlayOnStart;

	AudioComponent(bool playOnStart);
	AudioComponent(AudioComponent&) = delete;
	~AudioComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioComponent;

	virtual bool setup() override;
	void update();

	virtual AudioSource* getAudioSource() = 0;

	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	virtual String getName() const override { return "AudioComponent"; }
	ComponentID getComponentID() const { return s_ID; }
};