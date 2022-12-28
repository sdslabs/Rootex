#pragma once

#include "system.h"
#include "event_manager.h"

/// Input system responsible for registering and loading inputs.
class InputSystem : public System
{
	EventBinder<InputSystem> m_Binder;

	InputSystem();
	InputSystem(InputSystem&) = delete;

	Variant windowResized(const Event* event);

public:
	static InputSystem* GetSingleton();

	void loadSchemes(const HashMap<String, InputScheme>& schemes);
	void addScheme(const String& name, const InputScheme& scheme);
	void pushScheme(const String& name);
	void popScheme();
	void flushSchemes();

	bool initialize(const JSON::json& systemData) override;
	void setConfig(const SceneSettings& sceneSettings) override;
	void update(float deltaMilliseconds) override;
};
