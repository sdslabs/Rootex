#pragma once

#include "system.h"

/// Interface for initialisation, maintenance and deletion of script components.
class ScriptSystem : public System
{
	ScriptSystem();
	ScriptSystem(ScriptSystem&) = delete;
	~ScriptSystem() = default;

public:
	static ScriptSystem* GetSingleton();

	/// Calls OnBegin() function of script components.
	void begin() override;
	/// Calls OnUpdate() function of script components.
	void update(float deltaMilliseconds) override;
	/// Calls OnEnd() function of script components.
	void end();
};
