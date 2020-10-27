#pragma once

#include "system.h"
#include "event_manager.h"

class InputSystem : public System
{
	bool m_SchemeLock = false;

	InputSystem();
	InputSystem(InputSystem&) = delete;
	~InputSystem() = default;

	Variant windowResized(const Event* event);

public:
	static InputSystem* GetSingleton();

	void loadSchemes(const JSON::json& schemes);
	void setScheme(const String& scheme);
	void setSchemeLock(bool enabled);

	bool initialize(const JSON::json& systemData) override;
	void setConfig(const SceneSettings& sceneSettings) override;
	void update(float deltaMilliseconds) override;
};
