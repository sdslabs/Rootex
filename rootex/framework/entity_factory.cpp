#include "entity_factory.h"

#include "component.h"
#include "entity.h"
#include "system.h"

#include "components/test_component.h"
#include "components/debug_component.h"

EntityID EntityFactory::s_CurrentID = 1;

EntityFactory* EntityFactory::GetSingleton()
{
	static EntityFactory singleton;
	return &singleton;
}

EntityID EntityFactory::getNextID()
{
	return s_CurrentID++;
}

EntityFactory::EntityFactory()
{
	m_ComponentCreators["TestComponent"] = TestComponent::Create;
	m_ComponentCreators["DebugComponent"] = DebugComponent::Create;
}

Ref<Component> EntityFactory::createComponent(const String& name, const LuaVariable& componentData)
{
	auto findIt = m_ComponentCreators.find(name);
	if (findIt != m_ComponentCreators.end())
	{
		ComponentCreator create = findIt->second;
		Ref<Component> component(create(componentData));

		System::RegisterComponent(component.get());

		return component;
	}
	else
	{
		ERR("Could not find componentDescription: " + name);
		return nullptr;
	}
}

Ref<Entity> EntityFactory::createEntity(LuaTextResourceFile* actorLuaDescription)
{
	m_LuaState.loadExecuteScript(actorLuaDescription);

	LuaVariable entityDescription = m_LuaState.getGlobal("Entity");
	if (entityDescription.isNil())
	{
		ERR("Entity not found:" + actorLuaDescription->getPath().generic_string());
		return nullptr;
	}

	LuaVariable componentDescriptions = entityDescription["Components"];
	if (componentDescriptions.isNil())
	{
		ERR("Components not found while creating Entity:" + actorLuaDescription->getPath().generic_string());
		return nullptr;
	}

	Ref<Entity> entity;
	entity.reset(new Entity(getNextID()));

	for (auto& componentDescription : pairs(componentDescriptions))
	{
		if (componentDescription.first.type() == LUA_TSTRING)
		{
			Ref<Component> componentObject = createComponent(componentDescription.first, componentDescription.second);
			if (componentObject)
			{
				entity->addComponent(componentObject);
				componentObject->setOwner(entity);
			}
		}
	}

	return entity;
}
