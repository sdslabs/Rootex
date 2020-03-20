#include "script_component.h"

#include "resource_loader.h"
#include "entity.h"

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
{
}

ScriptComponent::~ScriptComponent()
{
}

bool ScriptComponent::setup()
{
	m_EntityTable = m_Owner->getName() + std::to_string(m_Owner->getID());
	LuaInterpreter::GetSingleton()->loadExecuteScript(m_EntityTable + " = loadEnv(\"" + m_ScriptFile->getPath().string() + "\")");
	return true;
}

void ScriptComponent::onBegin()
{
	LuaInterpreter::GetSingleton()->getGlobal(m_EntityTable)["onBegin"]();
}

void ScriptComponent::onUpdate(float deltaMilliSeconds)
{
	LuaInterpreter::GetSingleton()->getGlobal(m_EntityTable)["onUpdate"](deltaMilliSeconds, m_Owner.get());
}

void ScriptComponent::onEnd()
{
	LuaInterpreter::GetSingleton()->getGlobal(m_EntityTable)["onEnd"]();
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
	if (ImGui::BeginCombo("Script", m_ScriptFile->getPath().filename().string().c_str(), ImGuiComboFlags_HeightRegular))
	{
		for (auto&& file : ResourceLoader::GetFilesOfType(ResourceFile::Type::Lua))
		{
			if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
			{
				m_ScriptFile = (LuaTextResourceFile*)file;
			}
		}

		static String inputPath = "Path";
		ImGui::InputText("##Path", &inputPath);
		ImGui::SameLine();
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
}
#endif // ROOTEX_EDITOR
