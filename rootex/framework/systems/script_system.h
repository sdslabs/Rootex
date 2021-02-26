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

	/// Calls begin() function of script components.
	void begin() override;
	/// Calls update() function of script components.
	void update(float deltaMilliseconds) override;
	/// Calls end() function of script components.
	void end() override;
};
