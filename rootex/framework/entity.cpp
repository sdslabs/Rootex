#include "entity.h"

#include "event_manager.h"
#include "framework/ecs_factory.h"
#include "framework/component.h"
#include "framework/system.h"
#include "script/script.h"
#include "resource_loader.h"

#include "scene.h"

void Entity::RegisterAPI(sol::table& rootex)
{
	sol::usertype<Entity> entity = rootex.new_usertype<Entity>("Entity");
	entity["removeComponent"] = &Entity::removeComponent;
	entity["destroy"] = &Entity::destroy;
	entity["hasComponent"] = &Entity::hasComponent;
	entity["getScene"] = &Entity::getScene;
	entity["getName"] = &Entity::getName;
	entity["setScript"] = &Entity::setScript;
	entity["script"] = sol::property(&Entity::getScriptEnv, &Entity::setScriptEnv);

	sol::usertype<Component> component = rootex.new_usertype<Component>("Component");
	component["getOwner"] = &Component::getOwner;
	component["getComponentID"] = &Component::getComponentID;
	component["getName"] = &Component::getName;
}

Entity::Entity(Scene* scene, const JSON::json& script)
    : m_Scene(scene)
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

Entity::~Entity()
{
	destroy();
}

JSON::json Entity::getJSON() const
{
	JSON::json j;
	j["components"] = {};
	for (auto&& [componentID, component] : m_Components)
	{
		j["components"][component->getName()] = component->getJSON();
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

bool Entity::onAllComponentsAdded()
{
	bool status = true;
	for (auto& component : m_Components)
	{
		status = status & component.second->setup();
	}
	return status;
}

bool Entity::onAllEntitiesAdded()
{
	bool status = true;

	for (auto& component : m_Components)
	{
		status = status & component.second->setupEntities();
	}

	if (m_Script)
	{
		bool result = m_Script->setup();
		if (!result)
		{
			m_Script.reset();
			result = status;
		}
	}
	return status;
}

sol::table Entity::getScriptEnv()
{
	return (sol::table)m_Script->getScriptEnv();
}

void Entity::setScriptEnv(sol::table& changed)
{
	m_Script->setScriptEnv(changed);
}

void Entity::destroy()
{
	for (auto& component : m_Components)
	{
		component.second->onRemove();
		ECSFactory::DeregisterComponentInstance(component.second.get());
		component.second.reset();
	}
	m_Components.clear();
}

bool Entity::removeComponent(ComponentID toRemoveComponentID, bool hardRemove)
{
	Component* toRemoveComponent = getComponentFromID(toRemoveComponentID);
	if (!hardRemove)
	{
		for (auto& [componentID, component] : m_Components)
		{
			for (auto& dependency : component->getDependencies())
			{
				if (dependency->getID() == toRemoveComponentID)
				{
					WARN("Entity has other components depending on the to-be-removed component " + toRemoveComponent->getName());
					WARN("Component deletion denied");
					return false;
				}
			}
		}
	}

	toRemoveComponent->onRemove();
	ECSFactory::DeregisterComponentInstance(toRemoveComponent);
	m_Components.erase(toRemoveComponent->getComponentID());

	return true;
}

bool Entity::hasComponent(ComponentID componentID)
{
	return m_Components.find(componentID) != m_Components.end();
}

const HashMap<ComponentID, Ptr<Component>>& Entity::getAllComponents() const
{
	return m_Components;
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

bool Entity::setScript(const String& path)
{
	if (path.empty())
	{
		m_Script.reset();
		return true;
	}
	if (OS::IsExists(path))
	{
		JSON::json j;
		j["path"] = path;
		j["overrides"] = {};
		m_Script.reset(new Script(j));
		return m_Script->setup();
	}
	else
	{
		WARN("Could not find script file: " + path);
		return false;
	}
}

const String& Entity::getName() const
{
	return m_Scene->getName();
}

const String& Entity::getFullName() const
{
	return m_Scene->getFullName();
}

void Entity::draw() 
{
	ImGui::Text("Script");
	if (m_Script)
	{
		if (ImGui::Selectable(m_Script->getFilePath().c_str()))
		{
			EventManager::GetSingleton()->call("OpenScriptFile", "EditorOpenFile", m_Script->getFilePath());
		}
		if (ImGui::Button(ICON_ROOTEX_EXTERNAL_LINK "##Open Script"))
		{
			EventManager::GetSingleton()->call("OpenScriptFile", "EditorOpenFile", m_Script->getFilePath());
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_REFRESH "##Reload"))
		{
			JSON::json& j = m_Script->getJSON();
			m_Script.reset(new Script(j));
			m_Script->setup();
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_WINDOW_CLOSE "##RemoveScript"))
		{
			m_Script.reset();
		}
	}
	else
	{
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Choose Script"))
		{
			if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Lua), "game/assets/scripts/"))
			{
				setScript(*result);
			}
		}
	}

	if (m_Script)
	{
		m_Script->draw();
	}
}
