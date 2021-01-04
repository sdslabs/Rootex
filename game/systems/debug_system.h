#pragma once

#include "system.h"

class DebugSystem : public System
{
	DebugSystem();
	DebugSystem(DebugSystem&) = delete;
	~DebugSystem() = default;

public:
	static DebugSystem* GetSingleton();

	bool initialize(const JSON::json& systemData) override;
	void update(float deltaMilliseconds) override;
};
