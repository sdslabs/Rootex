#pragma once

#include "component.h"
#include "audio/audio_source.h"
#include "framework/components/transform_component.h"
#include "entity.h";

class AudioComponent : public Component
{

private:
	// add a member variable(object) of TransformComponent class 
	TransformComponent* m_TransformComponent;

protected:
	bool m_IsPlayOnStart;

	AudioComponent(bool playOnStart);
	AudioComponent(AudioComponent&) = delete;
	~AudioComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioComponent;
	
<<<<<<< HEAD
	virtual bool setup() override;
=======
	// override the setup function of component class
	virtual bool setup() override;
	// create update function
>>>>>>> 9eb42808fa2ad2a1f129f5dd0fedff749ab3a15e
	void update();

	virtual AudioSource* getAudioSource() = 0;

	bool isPlayOnStart() const { return m_IsPlayOnStart; }
	virtual String getName() const override { return "AudioComponent"; }
	ComponentID getComponentID() const { return s_ID; }
};
