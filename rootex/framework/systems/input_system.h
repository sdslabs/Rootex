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

	void loadSchemes(const HashMap<String, InputScheme>& schemes);
	void addScheme(const String& name, const InputScheme& inputScheme);
	void enableScheme(const String& scheme, bool enabled);
	void setSchemeLock(bool enabled);

	bool initialize(const JSON::json& systemData) override;
	void setConfig(const SceneSettings& sceneSettings) override;
	void update(float deltaMilliseconds) override;
};
