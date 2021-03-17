#pragma once

#include "component.h"

class TransformComponent;

/// Listening component which is used to put sound output to.
/// Useful for marking 3D sound attenuation in moving listeners
class AudioListenerComponent : public Component
{
	DEFINE_COMPONENT(AudioListenerComponent);
	DEPENDS_ON(TransformComponent);

public:
	AudioListenerComponent();
	~AudioListenerComponent() = default;

	Vector3 getPosition() const;

	void onRemove() override;
};
