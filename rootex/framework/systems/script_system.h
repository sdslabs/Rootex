#pragma once

#include "system.h"

/// Interface for initialisation, maintenance and deletion of script components.
class ScriptSystem : public System
{
	ScriptSystem();
	ScriptSystem(const ScriptSystem&) = delete;

	Vector<Entity*> m_ScriptEntitiesToInit;
	Vector<Entity*> m_ScriptEntitiesToEnter;

	sol::table m_FluxTweener;

public:
	static ScriptSystem* GetSingleton();

	void addInitScriptEntity(Entity* e);
	void removeInitScriptEntity(Entity* e);

	void addEnterScriptEntity(Entity* e);
	void removeEnterScriptEntity(Entity* e);

	/// Calls update() function of script components.
	void update(float deltaMilliseconds) override;
	/// Calls end() function of script components.
	void end() override;
};
