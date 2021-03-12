#pragma once

#include "system.h"

/// Interface for initialisation, maintenance and deletion of script components.
class ScriptSystem : public System
{
	ScriptSystem();
	ScriptSystem(ScriptSystem&) = delete;
	~ScriptSystem() = default;

	Vector<Entity*> m_ScriptEntitiesToInit;

public:
	static ScriptSystem* GetSingleton();

	void addInitScriptEntity(Entity* e);

	/// Calls update() function of script components.
	void update(float deltaMilliseconds) override;
	/// Calls end() function of script components.
	void end() override;
};
