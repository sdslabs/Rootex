#include "entity_factory.h"

#include "component.h"
#include "entity.h"
#include "system.h"

#include "systems/hierarchy_system.h"

#include "components/debug_component.h"
#include "components/test_component.h"
#include "components/transform_component.h"
#include "components/hierarchy_component.h"
#include "components/root_hierarchy_component.h"
#include "components/visual/visual_component.h"
#include "components/visual/diffuse_visual_component.h"
#include "components/visual/point_light_component.h"
#include "components/visual/directional_light_component.h"
#include "components/visual/spot_light_component.h"
#include "components/physics/sphere_component.h"
#include "components/physics/sphere_component.h"

#define REGISTER_COMPONENT(ComponentClass) \
m_ComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::Create }); \
m_DefaultComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::CreateDefault })

EntityID EntityFactory::s_CurrentID = ROOT_ENTITY_ID;

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
	REGISTER_COMPONENT(TransformComponent);
	REGISTER_COMPONENT(DiffuseVisualComponent);
	REGISTER_COMPONENT(PointLightComponent);
	REGISTER_COMPONENT(DirectionalLightComponent);
	REGISTER_COMPONENT(SpotLightComponent);
	REGISTER_COMPONENT(SphereComponent);
	REGISTER_COMPONENT(HierarchyComponent);
	REGISTER_COMPONENT(RootHierarchyComponent);
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
		ERR("Could not find componentDescription: " + name);
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
		ERR("Could not find componentDescription: " + name);
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
	m_Entities[root->m_ID] = root;
	
	Ref<RootHierarchyComponent> rootComponent(new RootHierarchyComponent(INVALID_ID, {}));

	root->addComponent(rootComponent);
	rootComponent->setOwner(root);

	root->addComponent(rootComponent->m_StaticGroup);
	rootComponent->m_StaticGroup->setOwner(root);
	
	root->addComponent(rootComponent->m_EntityGroup);
	rootComponent->m_EntityGroup->setOwner(root);
	
	root->addComponent(rootComponent->m_GlobalGroup);
	rootComponent->m_GlobalGroup->setOwner(root);
	
	root->addComponent(rootComponent->m_SkyGroup);
	rootComponent->m_SkyGroup->setOwner(root);
	
	root->addComponent(rootComponent->m_EditorGroup);
	rootComponent->m_EditorGroup->setOwner(root);

	System::RegisterComponent(rootComponent.get());

	return root;
}

void EntityFactory::addDefaultComponent(Ref<Entity> entity, String componentName)
{
	Ref<Component> component = createDefaultComponent(componentName);
	entity->addComponent(component);
	component->setOwner(entity);
	entity->setupComponents();
}

void EntityFactory::addComponent(Ref<Entity> entity, Ref<Component> component)
{
	entity->addComponent(component);
	component->setOwner(entity);
}

void EntityFactory::destroyEntities(bool saveRoot)
{
	Vector<Ref<Entity>> markedForRemoval;
	for (auto& entity : m_Entities)
	{
		if (entity.second)
		{
			if ((entity.second->getID() == ROOT_ENTITY_ID) && saveRoot)
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
		WARN("Destroyed entity: " + entity->getName());
		entity->destroy();
		m_Entities.erase(entity->getID());
	}
}
