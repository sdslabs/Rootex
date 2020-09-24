#include "script_component.h"

#include "entity.h"
#include "resource_loader.h"

Component* ScriptComponent::Create(const JSON::json& componentData)
{
	ScriptComponent* sc = new ScriptComponent(componentData["script"]);
	return sc;
}

Component* ScriptComponent::CreateDefault()
{
	ScriptComponent* sc = new ScriptComponent({});
	return sc;
}

ScriptComponent::ScriptComponent(const String& luaFilePath)
{
	if (!OS::IsExists(luaFilePath))
	{
		ERR("Could not find script file: " + luaFilePath);
		return;
	}

	addScript(luaFilePath);
}

ScriptComponent::~ScriptComponent()
{
}

bool ScriptComponent::setup()
{
	bool status = true;
	try
	{
		LuaInterpreter::GetSingleton()->getLuaState().script_file(m_ScriptFile, m_ScriptEnvironment);
	}
	catch (std::exception e)
	{
		ERR(e.what());
		status = false;
	}
	return status;
}

bool ScriptComponent::isSuccessful(const sol::function_result& result)
{
	if (!result.valid())
	{
		sol::error e = result;
		WARN("Script Execution failure in entity: " + m_Owner->getFullName());
		PRINT(e.what());
		return false;
	}
	return true;
}

void ScriptComponent::onBegin()
{
	if (m_ScriptFile != "")
	{
		isSuccessful(m_ScriptEnvironment["onBegin"](m_Owner));
	}
}

void ScriptComponent::onUpdate(float deltaMilliSeconds)
{
	if (m_ScriptFile != "")
	{
		isSuccessful(m_ScriptEnvironment["onUpdate"](m_Owner, deltaMilliSeconds));
	}
}

void ScriptComponent::onEnd()
{
	if (m_ScriptFile != "")
	{
		isSuccessful(m_ScriptEnvironment["onEnd"](m_Owner));
	}
}

void ScriptComponent::onHit(btPersistentManifold* manifold, PhysicsColliderComponent* other)
{
	isSuccessful(m_ScriptEnvironment["onHit"](m_Owner, manifold, other));
}

JSON::json ScriptComponent::getJSON() const
{
	JSON::json j;

	j["scripts"] = m_ScriptFile;

	return j;
}

void ScriptComponent::registerExports()
{
	sol::table currExports = m_ScriptEnvironment["exports"];
	std::filesystem::path file(m_ScriptFile);
	String script = file.stem().generic_string();

	currExports.for_each([&](sol::object const& key, sol::object const& value) {
		String varName = key.as<String>();
		m_IsOverriden[script + "." + varName] = false;
	});
}

bool ScriptComponent::addScript(const String& scriptFile)
{
	if (m_ScriptFile != "")
		return false;
	m_ScriptFile = scriptFile;
	m_ScriptEnvironment = sol::environment(LuaInterpreter::GetSingleton()->getLuaState(),
	    sol::create,
	    LuaInterpreter::GetSingleton()->getLuaState().globals());
	return true;
}

void ScriptComponent::removeScript()
{
	m_ScriptFile = "";
	m_IsOverriden.clear();
	m_Overrides.clear();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ScriptComponent::draw()
{
	ImGui::BeginGroup();
	ImGui::Text("Scripts");
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
			if (IsFileSupported(payloadPath.extension().string(), ResourceFile::Type::Lua))
			{
				if (addScript(payloadPath.string()))
				{
					registerExports();
				}
				else
				{
					WARN("A script already attached");
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

	std::filesystem::path file(m_ScriptFile);
	String fileName = file.stem().generic_string();

	ImGui::Text(fileName.c_str());
	auto currExports = m_ScriptEnvironment["exports"];
	if (currExports.valid())
	{
		sol::table exports = m_ScriptEnvironment["exports"];
		exports.for_each([&](sol::object const& key, sol::object const& value) {
			String varName = key.as<String>();
			ImGui::Text(varName.c_str());
			ImGui::SameLine();
			String qualifiedName = fileName + "." + varName;
			if (ImGui::Checkbox(String("##check" + qualifiedName).c_str(), &m_IsOverriden[qualifiedName]))
			{
				if (m_IsOverriden[qualifiedName])
				{
					m_Overrides[qualifiedName] = "";
					m_Overrides[qualifiedName].reserve(300);
				}
				else
				{
					m_Overrides.erase(qualifiedName);
				}
			}
			if (m_IsOverriden[qualifiedName])
			{
				ImGui::InputTextMultiline(String("##text" + qualifiedName).c_str(), &m_Overrides[qualifiedName], ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 1));
			}
			ImGui::Separator();
		});
	}
}
#endif // ROOTEX_EDITOR
