#pragma once

#include "system.h"

class InputSystem : public System
{
	InputSystem();
	InputSystem(InputSystem&) = delete;
	~InputSystem() = default;

public:
	static InputSystem* GetSingleton();

	bool initialize(const JSON::json& systemData) override;
	void update(float deltaMilliseconds) override;
};
