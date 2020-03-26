#include "script_component.h"

#include "entity.h"
#include "event_manager.h"
#include "resource_loader.h"

Component* ScriptComponent::Create(const JSON::json& componentData)
{
	ScriptComponent* sc = new ScriptComponent(ResourceLoader::CreateLuaTextResourceFile(componentData["script"]), componentData["connections"]);
	return sc;
}

Component* ScriptComponent::CreateDefault()
{
	ScriptComponent* sc = new ScriptComponent(ResourceLoader::CreateLuaTextResourceFile("rootex/assets/scripts/empty.lua"));
	return sc;
}

ScriptComponent::ScriptComponent(LuaTextResourceFile* luaFile, HashMap<String, String> connections)
    : m_ScriptFile(luaFile)
    , m_Env(LuaInterpreter::GetSingleton()->getLuaState(), sol::create, LuaInterpreter::GetSingleton()->getLuaState().globals())
    , m_Connections(connections)
{
}

ScriptComponent::~ScriptComponent()
{
}

bool ScriptComponent::setup()
{
	bool status = true;
	try
	{
		LuaInterpreter::GetSingleton()->getLuaState().script(m_ScriptFile->getString(), m_Env);

		m_Functions.clear();
		for (auto&& function : m_Env)
		{
			if (function.second.is<sol::function>())
			{
				m_Functions.push_back(function.first.as<String>());
			}
		}

		for (auto&& [function, eventType] : m_Connections)
		{
			connect(function, eventType);
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
		PRINT(e.what());
		return false;
	}
	return true;
}

void ScriptComponent::connect(const String& function, const String& eventType)
{
	String functionName = function;
	BIND_EVENT_FUNCTION(eventType, [=](const Event* event) {
		this->call(functionName);
		return true;
	});
	m_Connections[function] = eventType;
}

void ScriptComponent::call(const String& function)
{
	if (auto&& findIt = std::find(m_Functions.begin(), m_Functions.end(), function) != m_Functions.end())
	{
		isSuccessful(m_Env[function]());
	}
}

void ScriptComponent::onBegin()
{
	isSuccessful(m_Env["onBegin"](m_Owner));
}

void ScriptComponent::onUpdate(float deltaMilliSeconds)
{
	isSuccessful(m_Env["onUpdate"](deltaMilliSeconds, m_Owner));
}

void ScriptComponent::onEnd()
{
	isSuccessful(m_Env["onEnd"](m_Owner));
}

JSON::json ScriptComponent::getJSON() const
{
	JSON::json j;

	j["script"] = m_ScriptFile->getPath().string();
	j["connections"] = m_Connections;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ScriptComponent::draw()
{
	if (ImGui::BeginCombo("##Script", m_ScriptFile->getPath().filename().string().c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Lua))
		{
			if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
			{
				m_ScriptFile = (LuaTextResourceFile*)file;
			}
		}

		static String inputPath = "Path";
		ImGui::Separator();
		ImGui::InputText("##Path", &inputPath);
		ImGui::SameLine();
		if (ImGui::Button("Create Script"))
		{
			if (!ResourceLoader::CreateLuaTextResourceFile(inputPath))
			{
				WARN("Could not create script");
			}
			else
			{
				inputPath = "";
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	if (ImGui::Button("Script"))
	{
		EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_ScriptFile->getPath());
	}

	if (ImGui::ListBoxHeader("Functions", { 0, ImGui::GetTextLineHeightWithSpacing() * (m_Functions.size() + 1) }))
	{
		for (auto&& function : m_Functions)
		{
			if (ImGui::Selectable(function.c_str()))
			{
				EventManager::GetSingleton()->call("OpenScript", "EditorOpenFile", m_ScriptFile->getPath());
			}
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("ConnectEvent", &Pair<ScriptComponent*, String>(this, function), sizeof(Pair<ScriptComponent*, String>));
				ImGui::Text(function.c_str());
				ImGui::EndDragDropSource();
			}
		}
		ImGui::ListBoxFooter();
	}

	if (ImGui::ListBoxHeader("Connections", { 0, ImGui::GetTextLineHeightWithSpacing() * (m_Connections.size() + 2) }))
	{
		ImGui::Columns(2);
		ImGui::Text("Function");
		ImGui::NextColumn();
		ImGui::Text("Event");
		ImGui::NextColumn();

		ImGui::Separator();
		
		for (auto&& connection : m_Connections)
		{
			ImGui::Text(connection.first.c_str());
			ImGui::NextColumn();

			ImGui::Text(connection.second.c_str());
			ImGui::SameLine();
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::ListBoxFooter();
	}
}
#endif // ROOTEX_EDITOR
