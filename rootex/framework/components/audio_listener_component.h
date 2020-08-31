#pragma once

#include "component.h"

class TransformComponent;

/// Listening component which is used to put sound output to.
/// Useful for marking 3D sound attenuation in moving listeners
class AudioListenerComponent : public Component
{
	static Component* Create(const JSON::json& componentData);
	static Component* CreateDefault();

	friend class EntityFactory;

	TransformComponent* m_TransformComponent;

	AudioListenerComponent();
	AudioListenerComponent(const AudioListenerComponent&) = delete;
	~AudioListenerComponent() = default;

public:
	static const ComponentID s_ID = (ComponentID)ComponentIDs::AudioListenerComponent;

	bool setup() override;
	void onRemove() override;

	Vector3 getPosition() const;

	virtual String getName() const override { return "AudioListenerComponent"; }
	ComponentID getComponentID() const { return s_ID; }
};
