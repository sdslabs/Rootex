#pragma once

#include "component.h"

class TransformComponent;

/// Listening component which is used to put sound output to.
/// Useful for marking 3D sound attenuation in moving listeners
class AudioListenerComponent : public Component
{
	DEPENDS_ON(TransformComponent);

	static Component* Create(const JSON::json& componentData);

	friend class ECSFactory;

	AudioListenerComponent();
	AudioListenerComponent(const AudioListenerComponent&) = delete;
	~AudioListenerComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioListenerComponent;

	void onRemove() override;

	Vector3 getPosition() const;

	virtual const char* getName() const override { return "AudioListenerComponent"; }
	ComponentID getComponentID() const { return s_ID; }
};
