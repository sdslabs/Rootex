#pragma once

#include "script/interpreter.h"

class LuaTextResourceFile;

class Script
{
private:
	sol::environment m_ScriptEnvironment;
	String m_ScriptFile;
	HashMap<String, String> m_Overrides;

	bool isSuccessful(const sol::function_result& result);
public:
	Script(const JSON::json& script);
	Script(const Script&) = delete;
	~Script() = default;
	
	JSON::json getJSON() const;

	sol::environment& getScriptEnv() { return m_ScriptEnvironment; }
	void setScriptEnv(sol::table& env) { m_ScriptEnvironment.set(env); }
	bool call(const String& function, const Vector<Variant>& args);
	void evaluateOverrides();
	bool setup();

	const String& getFilePath() { return m_ScriptFile; }

	void draw();
};
