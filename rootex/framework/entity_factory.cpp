#include "entity_factory.h"

#include "component.h"
#include "entity.h"
#include "system.h"

#include "systems/hierarchy_system.h"

#include "components/debug_component.h"
#include "components/hierarchy_component.h"
#include "components/physics/sphere_component.h"
#include "components/script_component.h"
#include "components/test_component.h"
#include "components/transform_component.h"
#include "components/visual/diffuse_visual_component.h"
#include "components/visual/directional_light_component.h"
#include "components/visual/point_light_component.h"
#include "components/visual/spot_light_component.h"
#include "components/visual/text_visual_2d_component.h"
#include "components/visual/visual_component.h"

#define REGISTER_COMPONENT(ComponentClass)                                                            \
	m_ComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::Create }); \
	m_DefaultComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::CreateDefault })

EntityID EntityFactory::s_CurrentID = ROOT_ENTITY_ID;

void EntityFactory::BindFunctions()
{
	luabridge::getGlobalNamespace(LuaInterpreter::GetSingleton()->getLuaState())
	    .beginNamespace("Rootex")

		.beginClass<EntityFactory>("EntityFactory")
	    .addStaticFunction("GetSingleton", GetSingleton)
	    .addFunction("createEntity", Function<Entity*(EntityFactory*, String)>([](EntityFactory* e, String entityFile) { return e->createEntity(ResourceLoader::CreateTextResourceFile(entityFile)).get(); }))
	    .endClass()

	    .endNamespace();
}

EntityFactory* EntityFactory::GetSingleton()
{
	static EntityFactory singleton;
	return &singleton;
}

EntityID EntityFactory::getNextID()
{
	return ++s_CurrentID;
}

EntityFactory::EntityFactory()
{
	REGISTER_COMPONENT(TestComponent);
	REGISTER_COMPONENT(DebugComponent);
	REGISTER_COMPONENT(VisualComponent);
	REGISTER_COMPONENT(TextVisual2DComponent);
	REGISTER_COMPONENT(DiffuseVisualComponent);
	REGISTER_COMPONENT(TransformComponent);
	REGISTER_COMPONENT(PointLightComponent);
	REGISTER_COMPONENT(DirectionalLightComponent);
	REGISTER_COMPONENT(SpotLightComponent);
	REGISTER_COMPONENT(SphereComponent);
	REGISTER_COMPONENT(HierarchyComponent);
	REGISTER_COMPONENT(ScriptComponent);
}

EntityFactory::~EntityFactory()
{
	destroyEntities(false);
}

Ref<Component> EntityFactory::createComponent(const String& name, const JSON::json& componentData)
{
	auto& findIt = m_ComponentCreators.end();
	for (auto& componentClass = m_ComponentCreators.begin(); componentClass != m_ComponentCreators.end(); componentClass++)
	{
		if (Extract(String, *componentClass) == name)
		{
			findIt = componentClass;
		}
	}
	if (findIt != m_ComponentCreators.end())
	{
		ComponentCreator create = Extract(ComponentCreator, *findIt);
		Ref<Component> component(create(componentData));

		System::RegisterComponent(component.get());

		return component;
	}
	else
	{
		ERR("Could not find component creator: " + name);
		return nullptr;
	}
}

Ref<Component> EntityFactory::createDefaultComponent(const String& name)
{
	auto& findIt = m_DefaultComponentCreators.end();
	for (auto& componentClass = m_DefaultComponentCreators.begin(); componentClass != m_DefaultComponentCreators.end(); componentClass++)
	{
		if (Extract(String, *componentClass) == name)
		{
			findIt = componentClass;
		}
	}
	if (findIt != m_DefaultComponentCreators.end())
	{
		ComponentDefaultCreator create = Extract(ComponentDefaultCreator, *findIt);
		Ref<Component> component(create());

		System::RegisterComponent(component.get());

		return component;
	}
	else
	{
		ERR("Could not find default component creator: " + name);
		return nullptr;
	}
}

Ref<Entity> EntityFactory::createEntity(TextResourceFile* entityJSONDescription)
{
	const JSON::json entityJSON = JSON::json::parse(entityJSONDescription->getString());
	if (entityJSON.is_null())
	{
		ERR("Entity not found:" + entityJSONDescription->getPath().generic_string());
		return nullptr;
	}

	JSON::json componentJSON = entityJSON["Components"];
	if (componentJSON.is_null())
	{
		ERR("Components not found while creating Entity:" + entityJSONDescription->getPath().generic_string());
		return nullptr;
	}

	Ref<Entity> entity;
	JSON::json name = entityJSON["Entity"]["name"];

	EntityID newID = 0;
	auto&& findItID = entityJSON["Entity"].find("ID");
	if (findItID != entityJSON["Entity"].end())
	{
		newID = *findItID;
		while (getNextID() <= *findItID)
		{
			;
		}
	}
	else
	{
		newID = getNextID();
	}

	entity.reset(new Entity(newID, name.is_null() ? "Entity" : name));

	for (auto&& [componentName, componentDescription] : componentJSON.items())
	{
		Ref<Component> componentObject = createComponent(componentName, componentDescription);
		if (componentObject)
		{
			entity->addComponent(componentObject);
			componentObject->setOwner(entity);
		}
	}

	if (!entity->setupComponents())
	{
		ERR("Entity was not setup properly: " + std::to_string(entity->m_ID));
	}

	m_Entities[entity->m_ID] = entity;
	return entity;
}

Ref<Entity> EntityFactory::findEntity(EntityID entityID)
{
	auto&& findIt = m_Entities.find(entityID);

	if (findIt != m_Entities.end())
	{
		return findIt->second;
	}
	return nullptr;
}

Ref<Entity> EntityFactory::createRootEntity()
{
	Ref<Entity> root;
	root.reset(new Entity(ROOT_ENTITY_ID, "Root"));

	{
		Ref<HierarchyComponent> rootComponent(new HierarchyComponent(INVALID_ID, {}));
		EntityFactory::addComponent(root, rootComponent);
		System::RegisterComponent(rootComponent.get());
	}
	{
		Ref<Component> rootTransformComponent = createDefaultComponent("TransformComponent");
		EntityFactory::addComponent(root, rootTransformComponent);
		System::RegisterComponent(rootTransformComponent.get());
	}
	{
		Ref<VisualComponent> rootVisualComponent = std::dynamic_pointer_cast<VisualComponent>(createDefaultComponent("VisualComponent"));
		rootVisualComponent->setVisibility(false);
		EntityFactory::addComponent(root, rootVisualComponent);
		System::RegisterComponent(rootVisualComponent.get());
	}

	m_Entities[root->m_ID] = root;
	return root;
}

void EntityFactory::addDefaultComponent(Ref<Entity> entity, String componentName)
{
	addComponent(entity, createDefaultComponent(componentName));
}

void EntityFactory::addComponent(Ref<Entity> entity, Ref<Component> component)
{
	entity->addComponent(component);
	component->setOwner(entity);
	entity->setupComponents();
}

void EntityFactory::destroyEntities(bool saveRoot)
{
	Vector<Ref<Entity>> markedForRemoval;
	for (auto& entity : m_Entities)
	{
		if (entity.second)
		{
			if (entity.second->getID() == ROOT_ENTITY_ID)
			{
				if (saveRoot)
				{
					continue;
				}
			}

			if ((entity.second->getID() == INVALID_ID))
			{
				continue;
			}

			markedForRemoval.push_back(entity.second);
		}
		else
		{
			WARN("Found nullptr while browsing entities for destruction. Skipped during shutdown");
		}
	}

	for (auto&& entity : markedForRemoval)
	{
		entity->destroy();
		m_Entities.erase(entity->getID());
	}
}
