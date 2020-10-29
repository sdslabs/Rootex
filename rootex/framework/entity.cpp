#include "entity.h"

#include "event_manager.h"
#include "framework/component.h"
#include "framework/components/hierarchy_component.h"
#include "framework/system.h"
#include "framework/components/script_component.h"
#include "resource_loader.h"

void Entity::RegisterAPI(sol::table& rootex)
{
	sol::usertype<Entity> entity = rootex.new_usertype<Entity>("Entity");
	entity["removeComponent"] = &Entity::removeComponent;
	entity["destroy"] = &Entity::destroy;
	entity["hasComponent"] = &Entity::hasComponent;
	entity["getID"] = &Entity::getID;
	entity["getName"] = &Entity::getName;
	entity["setName"] = &Entity::setName;
	entity["setScript"] = &Entity::setScript;

	entity["script"] = sol::property(&Entity::getScriptEnv, &Entity::setScriptEnv);

	sol::usertype<Component> component = rootex.new_usertype<Component>("Component");
	component["getOwner"] = &Component::getOwner;
	component["getComponentID"] = &Component::getComponentID;
	component["getName"] = &Component::getName;
}

Entity::~Entity()
{
	destroy();
}

void Entity::addComponent(const Ref<Component>& component)
{
	m_Components.insert(std::make_pair(component->getComponentID(), component));
}

Entity::Entity(EntityID id, const String& name, const JSON::json& script)
    : m_ID(id)
    , m_Name(name)
    , m_IsEditorOnly(false)
{
	if (!script.is_null() && !script["path"].is_null())
	{
		if (OS::IsExists(script["path"]))
		{
			m_Script.reset(new Script(script));
		}
		else
		{
			ERR("Could not find script file: " + (String)script["path"]);
		}
	}
}

JSON::json Entity::getJSON() const
{
	JSON::json j;
	j["Entity"]["name"] = getName();
	j["Entity"]["ID"] = getID();
	j["Components"] = {};
	for (auto&& [componentID, component] : m_Components)
	{
		j["Components"][component->getName()] = component->getJSON();
	}
	if (m_Script)
	{
		j["Entity"]["script"] = m_Script->getJSON();
	}
	else
	{
		j["Entity"]["script"] = {};
	}

	return j;
}

bool Entity::setupComponents()
{
	bool status = true;
	for (auto& component : m_Components)
	{
		status = status & component.second->setup();
	}
	return status;
}

bool Entity::setupEntities()
{
	bool status = true;

	if (m_Script)
	{
		bool result = m_Script->setup();
		if (!result)
		{
			m_Script.reset();
		}
	}
	for (auto& component : m_Components)
	{
		status = status & component.second->setupEntities();
	}
	return status;
}

sol::table Entity::getScriptEnv()
{
	return (sol::table)m_Script->m_ScriptEnvironment;
}

void Entity::setScriptEnv(sol::table changed)
{
	m_Script->m_ScriptEnvironment.set(changed);
}

void Entity::destroy()
{
	for (auto& component : m_Components)
	{
		component.second->onRemove();
		System::DeregisterComponent(component.second.get());
		component.second.reset();
	}
	m_Components.clear();
}

void Entity::removeComponent(Ref<Component> component)
{
	component->onRemove();
	m_Components.erase(component->getComponentID());
	System::DeregisterComponent(component.get());
}

EntityID Entity::getID() const
{
	return m_ID;
}

const String& Entity::getName() const
{
	return m_Name;
}

String Entity::getFullName() const
{
	return m_Name + " #" + std::to_string(getID());
}

bool Entity::call(const String& function, const Vector<Variant>& args)
{
	bool status = false;
	if (m_Script)
	{
		status = m_Script->call(function, args);
		if (!status)
		{
			WARN("Script Execution failure in entity: " + getFullName());
		}
	}
	return status;
}

void Entity::evaluateScriptOverrides() 
{
	if (m_Script)
	{
		m_Script->evaluateOverrides();
	}
};

bool Entity::setScriptInternal(const String& path)
{
	JSON::json j;
	j["path"] = path;
	j["overrides"] = {};
	m_Script.reset(new Script(j));
	return m_Script->setup();
}

bool Entity::setScript(const String& path)
{
	if (path.empty())
	{
		m_Script.reset();
		return true;
	}
	if (OS::IsExists(path))
	{
		bool status = setScriptInternal(path);
		call("onBegin", { Ref<Entity>(this) });
		return true;
	}
	else
	{
		WARN("Could not find script file: " + path);
		return false;
	}
}

bool Entity::hasComponent(ComponentID componentID)
{
	return m_Components.find(componentID) != m_Components.end();
}

void Entity::setName(const String& name)
{
	m_Name = name;
}

const HashMap<ComponentID, Ref<Component>>& Entity::getAllComponents() const
{
	return m_Components;
}

#ifdef ROOTEX_EDITOR
void Entity::draw() 
{
	ImGui::BeginGroup();
	ImGui::Text("Script");

	if (m_Script)
	{
		if (ImGui::Button("X"))
		{
			m_Script.reset();
		}
		ImGui::SameLine();
		//TODO: replace with proper icon
		if (ImGui::Button("Reload"))
		{
			JSON::json& j = m_Script->getJSON();
			m_Script.reset(new Script(j));
			m_Script->setup();
		}
		ImGui::SameLine();
		if (ImGui::Selectable(m_Script->m_ScriptFile.c_str()))
		{
			EventManager::GetSingleton()->call("OpenScriptFile", "EditorOpenFile", m_Script->m_ScriptFile);
		}
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
				setScriptInternal(payloadPath.generic_string());
			}
			else
			{
				WARN("Cannot assign a non-lua file as Script");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (m_Script)
	{
		m_Script->draw();
	}
};
#endif
