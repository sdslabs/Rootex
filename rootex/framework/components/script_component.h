#pragma once

#include "component.h"
#include "event_manager.h"
#include "script/interpreter.h"
#include "physics/physics_collider_component.h"
#include "entity.h"

#include "btBulletDynamicsCommon.h"

class LuaTextResourceFile;

class Script
{
private:
	sol::environment m_ScriptEnvironment;
	String m_ScriptFile;
	HashMap<String, bool> m_IsOverriden;
	HashMap<String, String> m_Overrides;
	Entity* m_Entity;
	bool hasScript;

	friend class Entity;

	bool isSuccessful(const sol::function_result& result);

public:
	bool setup(Entity* entity);

	JSON::json getJSON() const;

	bool addScript(const String& scriptFile);
	void removeScript();
	bool call(String function, Vector<Variant> args);
	sol::environment getEnvironment() { return m_ScriptEnvironment; }
	void evaluateOverrides();
	
	Script(const JSON::json& script);
	Script(const Script&) = delete;
	~Script() = default;

#ifdef ROOTEX_EDITOR
	void registerExports();
	void draw();
#endif
};
