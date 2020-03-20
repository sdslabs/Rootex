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

void ScriptComponent::begin()
{
	LuaInterpreter::GetSingleton()->getGlobal(m_EntityTable)["onBegin"]();
}

void ScriptComponent::update(float deltaMilliSeconds)
{
	LuaInterpreter::GetSingleton()->getGlobal(m_EntityTable)["onUpdate"](deltaMilliSeconds);
}

void ScriptComponent::end()
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
#include "editor/gui/resource_selector.h"
void ScriptComponent::draw()
{
	ResourceFile* selected = ResourceSelector::DrawSelect({ ResourceFile::Type::Lua });
	if (selected)
	{
		m_ScriptFile = (LuaTextResourceFile*)selected;
	}
}
#endif // ROOTEX_EDITOR
