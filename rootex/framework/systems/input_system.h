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

	void loadSchemes(const JSON::json& schemes);
	void setScheme(const String& scheme);

	bool initialize(const JSON::json& systemData) override;
	void setConfig(const JSON::json& configData, bool openInEditor) override;
	void update(float deltaMilliseconds) override;
};
