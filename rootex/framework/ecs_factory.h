#pragma once

#include "common/common.h"
#include "core/resource_files/text_resource_file.h"

#include "scene.h"
#include "entity.h"

#include "utility/component_array.h"

typedef unsigned int ComponentID;
class Component;

class BaseComponentSet
{
public:
	virtual bool addComponent(Entity& owner, const JSON::json& componentData, bool checks = true) = 0;
	virtual bool addDefaultComponent(Entity& owner, bool checks) = 0;
	virtual bool removeComponent(Entity& entity) = 0;
	virtual const String& getName() const = 0;
	virtual const String& getCategory() const = 0;
	virtual const ComponentID& getID() const = 0;
};

template <class T>
class ComponentSet : public BaseComponentSet
{
	ComponentArray<T> m_Instances;

public:
	ComponentSet() = default;
	ComponentSet(const ComponentSet&) = delete;
	ComponentSet& operator=(const ComponentSet&) = delete;

	ComponentArray<T>& getAll() { return m_Instances; }

	bool addComponent(Entity& owner, const JSON::json& componentData, bool checks) override
	{
		if (!owner.hasComponent(T::s_ID))
		{
			m_Instances.emplace_back( owner, componentData);

			if (checks && !owner.onAllComponentsAdded())
			{
				if (owner.hasComponent(T::s_ID))
				{
					owner.removeComponent(T::s_ID, true);
				}
				return false;
			}
			return true;
		}

		WARN("Entity already has a " + T::s_Name + ": " + owner.getFullName());
		return false;
	}

	bool addDefaultComponent(Entity& owner, bool checks) override
	{
		return addComponent(owner, JSON::json::object(), checks);
	}

	bool removeComponent(Entity& entity) override
	{
		auto& findIt = std::find_if(m_Instances.begin(), m_Instances.end(), [&entity](T& c) {
			return c.getOwner().getID() == entity.getID();
		});

		if (findIt != m_Instances.end())
		{
			findIt->onRemove();
			//int index = (int)(findIt - m_Instances.begin());
			m_Instances.erase(findIt);
			return true;
		}
		return false;
	}

	const String& getName() const override { return T::s_Name; };
	const String& getCategory() const override { return T::s_Category; };
	const ComponentID& getID() const override { return T::s_ID; };
};

namespace ECSFactory
{
extern HashMap<String, Ptr<BaseComponentSet>> s_ComponentSets;

void Initialize();

void FillEntity(Entity& entity, const JSON::json& entityJSON);
void FillEntityFromFile(Entity& entity, TextResourceFile* textResourceFile);
void FillRootEntity(Entity& root);

void CopyEntity(Entity& entity, Entity& copyTarget);
String GetComponentNameByID(ComponentID componentID);
ComponentID GetComponentIDByName(const String& componentName);

bool AddComponent(Entity& entity, ComponentID componentID, const JSON::json& componentData, bool checks = true);
bool AddDefaultComponent(Entity& entity, ComponentID componentID, bool checks = true);
bool RemoveComponent(Entity& entity, ComponentID componentID);
};
