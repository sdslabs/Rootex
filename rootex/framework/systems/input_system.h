#pragma once

#include "system.h"
#include "event_manager.h"

class InputSystem : public System
{
	InputSystem();
	InputSystem(InputSystem&) = delete;
	~InputSystem() = default;

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
