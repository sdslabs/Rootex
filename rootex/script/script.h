#pragma once

#include "script/interpreter.h"

class LuaTextResourceFile;

class Script
{
private:
	sol::table m_ScriptInstance;
	String m_ScriptFile;
	HashMap<String, String> m_Overrides;

	bool isSuccessful(const sol::function_result& result);

public:
	Script(const JSON::json& script);
	Script(const Script&) = delete;
	~Script() = default;

	bool setup(Entity* entity);

	void evaluateOverrides();
	bool call(const String& function, const Vector<Variant>& args);

	JSON::json getJSON() const;
	const String& getFilePath() { return m_ScriptFile; }
	sol::table& getScriptInstance() { return m_ScriptInstance; }

	void draw();
};
