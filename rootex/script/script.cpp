#include "script.h"
#include "event_manager.h"
#include "scene.h"
#include "components/physics/hit.h"

Script::Script(const JSON::json& script)
{
	String luaFilePath = script["path"];

	m_ScriptFile = luaFilePath;
	for (auto& element : JSON::json::iterator_wrapper(script["overrides"]))
	{
		m_Overrides[element.key()] = (String)element.value();
	}
}

bool Script::setup(Entity* entity)
{
	bool status = true;
	try
	{
		sol::table scriptClass = LuaInterpreter::GetSingleton()->getLuaState().script_file(m_ScriptFile);
		m_ScriptInstance = scriptClass["new"](scriptClass, entity);
	}
	catch (std::exception e)
	{
		WARN(e.what());
		m_ScriptInstance = LuaInterpreter::GetSingleton()->getLuaState().create_named_table("DefaultTable");
		status = false;
	}
	return status;
}

bool Script::isSuccessful(const sol::function_result& result)
{
	if (!result.valid())
	{
		sol::error e = result;
		WARN(e.what());
		return false;
	}
	return true;
}

bool Script::call(const String& function, const Vector<Variant>& args)
{
	if (!m_ScriptInstance[function].valid())
	{
		return true;
	}

	bool status = isSuccessful(m_ScriptInstance[function](m_ScriptInstance, sol::as_args(args)));
	if (!status)
	{
		WARN("Could not call " + function);
	}
	return status;
}

void Script::evaluateOverrides()
{
	for (auto&& [varName, lua] : m_Overrides)
	{
		if (!lua.empty())
		{
			sol::optional<sol::object> currVar = m_ScriptInstance["exports"][varName];
			if (currVar)
			{
				m_ScriptInstance["exports"][varName] = LuaInterpreter::GetSingleton()->getLuaState().script("return " + lua);
			}
		}
	}
}

JSON::json Script::getJSON() const
{
	JSON::json j;

	j["path"] = m_ScriptFile;

	j["overrides"] = {};

	sol::optional<sol::table> currExports = m_ScriptInstance["exports"];
	if (currExports)
	{
		sol::table exports = m_ScriptInstance["exports"];
		exports.for_each([&](sol::object const& key, sol::object const& value) {
			String varName = key.as<String>();
			if (m_Overrides.find(varName) != m_Overrides.end())
			{
				j["overrides"][varName] = m_Overrides.at(varName);
			}
		});
	}

	return j;
}

void Script::draw()
{
	ImGui::Text("Script Exports");

	sol::optional<sol::table> currExports = m_ScriptInstance["exports"];
	if (currExports)
	{
		sol::table exports = *currExports;

		ImGui::LabelText("Variable", "Overriding Lua");
		exports.for_each([&](sol::object const& key, sol::object const& value) {
			String varName = key.as<String>();
			if (m_Overrides.find(varName) == m_Overrides.end())
			{
				m_Overrides[varName] = "";
			}
			ImGui::InputText(varName.c_str(), &m_Overrides[varName]);
		});
	}
	ImGui::Separator();
}
