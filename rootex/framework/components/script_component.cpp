#include "script_component.h"

Script::Script(const JSON::json& script)
{
	String luaFilePath = script["path"];

	for (auto& element : JSON::json::iterator_wrapper(script["overrides"]))
	{
		m_Overrides[element.key()] = (String)element.value();
		m_IsOverriden[element.key()] = true;
	}

	addScriptInternal(luaFilePath);
}

bool Script::setup()
{
	bool status = true;
	try
	{
		LuaInterpreter::GetSingleton()->getLuaState().script_file(m_ScriptFile, m_ScriptEnvironment);
	}
	catch (std::exception e)
	{
		WARN(e.what());
		status = false;
	}
	return status;
}

bool Script::isSuccessful(const sol::function_result& result)
{
	if (!result.valid())
	{
		sol::error e = result;
		PRINT(e.what());
		return false;
	}
	return true;
}

bool Script::call(const String& function, const Vector<Variant>& args) 
{
	return isSuccessful(m_ScriptEnvironment[function](sol::as_args(args)));
}

void Script::evaluateOverrides()
{
	for (auto&& [varName, lua] : m_Overrides)
	{
		sol::optional<sol::object> currVar = m_ScriptEnvironment["exports"][varName];
		if (currVar)
		{
			m_ScriptEnvironment["exports"][varName] = LuaInterpreter::GetSingleton()->getLuaState().script("return " + lua);
		}
	}
}

JSON::json Script::getJSON() const
{
	JSON::json j;

	j["path"] = m_ScriptFile;

	j["overrides"] = {};

	for (auto&& [key, value] : m_Overrides)
	{
		j["overrides"][key] = value;
	}

	return j;
}

bool Script::addScriptInternal(const String& scriptFile)
{
	if (!m_ScriptFile.empty())
	{
		return false;
	}
	m_ScriptFile = scriptFile;
	m_ScriptEnvironment = sol::environment(LuaInterpreter::GetSingleton()->getLuaState(),
	    sol::create,
	    LuaInterpreter::GetSingleton()->getLuaState().globals());
	return true;
}

#ifdef ROOTEX_EDITOR

void Script::registerExports()
{
	auto exportsCheck = m_ScriptEnvironment["exports"];
	if (exportsCheck.valid())
	{
		sol::table exports = m_ScriptEnvironment["exports"];
		sol::table currExports = m_ScriptEnvironment["exports"];
		FilePath file(m_ScriptFile);
		String script = file.stem().generic_string();

		currExports.for_each([&](sol::object const& key, sol::object const& value) {
			String varName = key.as<String>();
			m_IsOverriden[varName] = false;
		});
	}
}

#include "imgui.h"
#include "imgui_stdlib.h"
void Script::draw()
{

	ImGui::Text("Script Exports");

	sol::optional<sol::table> currExports = m_ScriptEnvironment["exports"];
	if (currExports)
	{
		sol::table exports = m_ScriptEnvironment["exports"];
		exports.for_each([&](sol::object const& key, sol::object const& value) {
			String varName = key.as<String>();
			ImGui::Text(varName.c_str());
			ImGui::SameLine();
			if (ImGui::Checkbox(String("##check" + varName).c_str(), &m_IsOverriden[varName]))
			{
				if (m_IsOverriden[varName])
				{
					m_Overrides[varName] = "";
				}
				else
				{
					m_Overrides.erase(varName);
				}
			}
			if (m_IsOverriden[varName])
			{
				ImGui::InputTextMultiline(String("##text" + varName).c_str(), &m_Overrides[varName], ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 1));
			}
			ImGui::Separator();
		});
	}
}
#endif // ROOTEX_EDITOR
