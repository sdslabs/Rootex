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

	friend class Entity;

	bool addScriptInternal(const String& scriptFile);
	bool isSuccessful(const sol::function_result& result);
public:
	Script(const JSON::json& script);
	Script(const String& filePath);
	Script(const Script&) = delete;
	~Script() = default;
	
	bool setup();

	JSON::json getJSON() const;

	bool call(const String& function, const Vector<Variant>& args);
	sol::environment getEnvironment() { return m_ScriptEnvironment; }
	void evaluateOverrides();

#ifdef ROOTEX_EDITOR
	void registerExports();
	void draw();
#endif
};
