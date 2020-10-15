#include "script_component.h"

#include "resource_loader.h"

Script::Script(const JSON::json& script)
	: m_Entity(nullptr)
{
	if (!script.is_null() && !script["path"].is_null())
	{
		String luaFilePath = script["path"];

		if (OS::IsExists(luaFilePath))
		{
			for (auto& element : JSON::json::iterator_wrapper(script["overrides"]))
			{
				m_Overrides[element.key()] = (String)element.value();
				m_IsOverriden[element.key()] = true;
			}

			addScript(luaFilePath);
			return;
		}
		else
		{
			ERR("Could not find script file: " + luaFilePath);
		}
	}
	m_HasScript = false;
}

bool Script::setup(Entity* entity)
{
	bool status = true;
	m_Entity = entity;
	if (m_HasScript)
	{
		try
		{
			LuaInterpreter::GetSingleton()->getLuaState().script_file(m_ScriptFile, m_ScriptEnvironment);
		}
		catch (std::exception e)
		{
			ERR(e.what());
			status = false;
			m_HasScript = false;
		}
	}
	return status;
}

bool Script::isSuccessful(const sol::function_result& result)
{
	if (!result.valid())
	{
		sol::error e = result;
		WARN("Script Execution failure in entity: " + m_Entity->getFullName());
		PRINT(e.what());
		return false;
	}
	return true;
}

bool Script::call(const String& function, const Vector<Variant>& args) 
{
	if (m_HasScript)
	{
		return isSuccessful(m_ScriptEnvironment[function](sol::as_args(args)));
	}
	return false;
}

void Script::evaluateOverrides()
{
	for (auto&& [varName, lua] : m_Overrides)
	{
		auto currVar = m_ScriptEnvironment["exports"][varName];
		if (currVar.valid())
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

bool Script::addScript(const String& scriptFile)
{
	if (!m_ScriptFile.empty())
	{
		return false;
	}
	m_ScriptFile = scriptFile;
	m_ScriptEnvironment = sol::environment(LuaInterpreter::GetSingleton()->getLuaState(),
	    sol::create,
	    LuaInterpreter::GetSingleton()->getLuaState().globals());
	m_HasScript = true;
	return true;
}

void Script::removeScript()
{
	m_HasScript = false;
	m_ScriptFile = "";
	m_IsOverriden.clear();
	m_Overrides.clear();
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
	ImGui::BeginGroup();
	ImGui::Text("Script");
	if (ImGui::Button("X"))
	{
		removeScript();
	}
	ImGui::SameLine();
	if (ImGui::Selectable(m_ScriptFile.c_str()))
	{
		EventManager::GetSingleton()->call("OpenScriptFile", "EditorOpenFile", m_ScriptFile);
	}

	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (IsFileSupported(payloadPath.extension().generic_string(), ResourceFile::Type::Lua))
			{
				if (addScript(payloadPath.string()))
				{
					registerExports();
				}
				else
				{
					WARN("A script is already attached");
				}
			}
			else
			{
				WARN("Cannot assign a non-lua file as Script");
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Text("Script Exports");

	auto currExports = m_ScriptEnvironment["exports"];
	if (currExports.valid())
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
