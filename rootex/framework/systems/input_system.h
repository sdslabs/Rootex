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

	bool initialize(const JSON::json& systemData) override;
	void update(float deltaMilliseconds) override;
};
