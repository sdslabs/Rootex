#include "script_component.h"

#include "entity.h"
#include "event_manager.h"
#include "resource_loader.h"

Component* ScriptComponent::Create(const JSON::json& componentData)
{
	ScriptComponent* sc = new ScriptComponent(ResourceLoader::CreateLuaTextResourceFile(componentData["script"]));
	return sc;
}

Component* ScriptComponent::CreateDefault()
{
	ScriptComponent* sc = new ScriptComponent(ResourceLoader::CreateLuaTextResourceFile("rootex/assets/scripts/empty.lua"));
	return sc;
}

ScriptComponent::ScriptComponent(LuaTextResourceFile* luaFile)
    : m_ScriptFile(luaFile)
    , m_Env(LuaInterpreter::GetSingleton()->getLuaState(), sol::create, LuaInterpreter::GetSingleton()->getLuaState().globals())
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
	}
	catch (std::exception e)
	{
		ERR(e.what());
		status = false;
	}
	return status;
}

void ScriptComponent::isSuccessful(const sol::function_result& result)
{
	if (!result.valid())
	{
		sol::error e = result;
		PRINT(e.what());
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

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void ScriptComponent::draw()
{
	ImGui::BeginGroup();
	if (ImGui::BeginCombo("##Script", m_ScriptFile->getPath().filename().string().c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Lua))
		{
			if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
			{
				setScript((LuaTextResourceFile*)file);
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
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (payloadPath.extension() == ".lua")
			{
				setScript(ResourceLoader::CreateLuaTextResourceFile(payloadPath.string()));
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
