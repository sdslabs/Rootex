#include "entity.h"

#include "event_manager.h"
#include "framework/ecs_factory.h"
#include "framework/component.h"
#include "framework/system.h"
#include "framework/systems/script_system.h"
#include "script/script.h"
#include "resource_loader.h"

Entity::Entity(Scene* scene)
    : m_Scene(scene)
{
}

Entity::~Entity()
{
	ScriptSystem::GetSingleton()->removeInitScriptEntity(this);
	ScriptSystem::GetSingleton()->removeEnterScriptEntity(this);

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
		j["script"] = m_Script->getJSON();
	}
	else
	{
		j["script"] = {};
	}

	return j;
}

bool Entity::onAllComponentsAdded()
{
	bool status = true;
	for (auto& component : m_Components)
	{
		status = status && component.second->setup();
	}
	return status;
}

bool Entity::onAllEntitiesAdded()
{
	bool status = true;

	for (auto& component : m_Components)
	{
		status = status && component.second->setupEntities();
	}

	if (m_Script)
	{
		bool result = m_Script->setup(this);
		if (!result)
		{
			m_Script.reset();
			result = status;
		}
	}
	return status;
}

void Entity::clear()
{
	for (auto& [id, component] : m_Components)
	{
		ECSFactory::RemoveComponent(*this, id);
	}
	m_Components.clear();
}

void Entity::destroy()
{
	call("destroy", { this });

	for (auto& [componentID, component] : m_Components)
	{
		ECSFactory::RemoveComponent(*this, componentID);
	}
	m_Components.clear();
}

bool Entity::addDefaultComponent(const String& componentName)
{
	return ECSFactory::AddDefaultComponent(*this, ECSFactory::GetComponentIDByName(componentName));
}

bool Entity::addComponent(const String& componentName, const JSON::json& componentData)
{
	return ECSFactory::AddComponent(*this, ECSFactory::GetComponentIDByName(componentName), componentData);
}

void Entity::registerComponent(Component* component)
{
	m_Components[component->getComponentID()] = component;
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
					WARN("Entity has other components depending on the to-be-removed component (" + toRemoveComponent->getName() + "). Component deletion was denied");
					return false;
				}
			}
		}
	}

	ECSFactory::RemoveComponent(*this, toRemoveComponentID);
	m_Components.erase(toRemoveComponentID);

	return true;
}

bool Entity::hasComponent(ComponentID componentID)
{
	return m_Components.find(componentID) != m_Components.end();
}

const HashMap<ComponentID, Component*>& Entity::getAllComponents() const
{
	return m_Components;
}

void Entity::bind(const Event::Type& event, const sol::function& function)
{
	m_Binder.bind(event, [this, function](const Event* e) -> Variant
	    { return function.call<Variant>(m_Script->getScriptInstance(), this, e); });
}

bool Entity::call(const String& function, const Vector<Variant>& args)
{
	bool status = false;
	if (m_Script)
	{
		status = m_Script->call(function, args);
		if (!status)
		{
			WARN("Script error in " + m_Script->getFilePath() + " on " + getFullName() + " during " + function);
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

bool Entity::setScriptJSON(const JSON::json& script)
{
	if (script.contains("path"))
	{
		if (OS::IsExists(script["path"]))
		{
			m_Script.reset(new Script(script));
			m_Script->setup(this);
			ScriptSystem::GetSingleton()->addInitScriptEntity(this);
			return true;
		}
		else
		{
			ERR("Could not find script file: " + (String)script["path"]);
			return false;
		}
	}
	return false;
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
		JSON::json j;
		j["path"] = path;
		j["overrides"] = {};
		m_Script.reset(new Script(j));
		ScriptSystem::GetSingleton()->addInitScriptEntity(this);
		return m_Script->setup(this);
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

const SceneID Entity::getID() const
{
	return m_Scene->getID();
}

const String& Entity::getFullName() const
{
	return m_Scene->getFullName();
}

void Entity::draw()
{
	ImGui::BeginGroup();
	ImGui::Text("Script");
	if (m_Script)
	{
		if (ImGui::Selectable(m_Script->getFilePath().c_str()))
		{
			EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_Script->getFilePath(), (int)ResourceFile::Type::Text });
		}
		if (ImGui::Button(ICON_ROOTEX_EXTERNAL_LINK "##Open Script"))
		{
			EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_Script->getFilePath(), (int)ResourceFile::Type::Text });
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_PENCIL_SQUARE_O "##Edit Script"))
		{
			EventManager::GetSingleton()->call(EditorEvents::EditorEditFile, m_Script->getFilePath());
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_REFRESH "##Reload"))
		{
			JSON::json& j = m_Script->getJSON();
			m_Script.reset(new Script(j));
			m_Script->setup(this);
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_WINDOW_CLOSE "##RemoveScript"))
		{
			m_Script.reset();
		}
		if (m_Script)
		{
			m_Script->draw();
		}
	}
	else
	{
		ImGui::SameLine();
		if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Choose Script"))
		{
			if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Lua), "game/assets/scripts/"))
			{
				setScript(*result);
			}
		}
	}
	ImGui::EndGroup();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_PAYLOAD"))
		{
			const char* path = (const char*)payload->Data;
			setScript(path);
		}
	}
}
