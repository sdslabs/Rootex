#include "script_component.h"

#include "entity.h"
#include "resource_loader.h"

Component* ScriptComponent::Create(const JSON::json& componentData)
{
	ScriptComponent* sc = new ScriptComponent(componentData["scripts"]);
	return sc;
}

Component* ScriptComponent::CreateDefault()
{
	ScriptComponent* sc = new ScriptComponent({});
	return sc;
}

ScriptComponent::ScriptComponent(const Vector<String>& luaFilePaths)
{
	for (auto& path : luaFilePaths)
	{
		if (!OS::IsExists(path))
		{
			ERR("Could not find script file: " + path);
			continue;
		}

		addScript(path);
	}
}

ScriptComponent::~ScriptComponent()
{
}

bool ScriptComponent::setup()
{
	bool status = true;
	try
	{
		for (int i = 0; i < m_ScriptFiles.size(); i++)
		{
			LuaInterpreter::GetSingleton()->getLuaState().script_file(m_ScriptFiles[i], m_ScriptEnvironments[i]);
		}
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
	for (auto& env : m_ScriptEnvironments)
	{
		isSuccessful(env["onBegin"](m_Owner));
	}
}

void ScriptComponent::onUpdate(float deltaMilliSeconds)
{
	for (auto& env : m_ScriptEnvironments)
	{
		isSuccessful(env["onUpdate"](m_Owner, deltaMilliSeconds));
	}
}

void ScriptComponent::onEnd()
{
	for (auto& env : m_ScriptEnvironments)
	{
		isSuccessful(env["onEnd"](m_Owner));
	}
}

void ScriptComponent::onHit(btPersistentManifold* manifold, PhysicsColliderComponent* other)
{
	for (auto& env : m_ScriptEnvironments)
	{
		isSuccessful(env["onHit"](m_Owner, manifold, other));
	}
}

JSON::json ScriptComponent::getJSON() const
{
	JSON::json j;

	j["scripts"] = JSON::json::array();
	for (auto& scriptFile : m_ScriptFiles)
	{
		j["scripts"].push_back(scriptFile);
	}

	return j;
}

void ScriptComponent::addScript(const String& scriptFile)
{
	m_ScriptFiles.push_back(scriptFile);
	m_ScriptEnvironments.push_back(
	    sol::environment(LuaInterpreter::GetSingleton()->getLuaState(),
	        sol::create,
	        LuaInterpreter::GetSingleton()->getLuaState().globals()));
}

void ScriptComponent::removeScript(const String& scriptFile)
{
	for (int i = 0; i < m_ScriptFiles.size(); i++)
	{
		if (scriptFile == m_ScriptFiles[i])
		{
			m_ScriptFiles.erase(m_ScriptFiles.begin() + i);
			m_ScriptEnvironments.erase(m_ScriptEnvironments.begin() + i);
		}
	}
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ScriptComponent::draw()
{
	ImGui::BeginGroup();
	if (ImGui::ListBoxHeader("Scripts", m_ScriptFiles.size()))
	{
		for (auto& scriptFile : m_ScriptFiles)
		{
			if (ImGui::Button("X"))
			{
				removeScript(scriptFile);
			}
			ImGui::SameLine();
			if (ImGui::Selectable(scriptFile.c_str()))
			{
				EventManager::GetSingleton()->call("OpenScriptFile", "EditorOpenFile", scriptFile);
			}
		}
		ImGui::ListBoxFooter();
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
				addScript(payloadPath.string());
			}
			else
			{
				WARN("Cannot assign a non-lua file as Script");
			}
		}
		ImGui::EndDragDropTarget();
	}
}
#endif // ROOTEX_EDITOR
