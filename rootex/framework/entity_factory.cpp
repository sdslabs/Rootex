#include "entity_factory.h"

#include "component.h"
#include "entity.h"
#include "system.h"

#include "systems/hierarchy_system.h"

#include "components/debug_component.h"
#include "components/test_component.h"
#include "components/transform_component.h"
#include "components/hierarchy_component.h"
#include "components/visual/visual_component.h"
#include "components/visual/diffuse_visual_component.h"
#include "components/physics/sphere_component.h"
#include "components/visual/point_light_component.h"
#include "components/visual/directional_light_component.h"
#include "components/visual/spot_light_component.h"
#include "components/physics/sphere_component.h"

#define REGISTER_COMPONENT(ComponentClass) \
m_ComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::Create }); \
m_DefaultComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::CreateDefault })

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
	REGISTER_COMPONENT(TestComponent);
	REGISTER_COMPONENT(DebugComponent);
	REGISTER_COMPONENT(VisualComponent);
	REGISTER_COMPONENT(TransformComponent);
	REGISTER_COMPONENT(DiffuseVisualComponent);
	REGISTER_COMPONENT(PointLightComponent);
	REGISTER_COMPONENT(DirectionalLightComponent);
	REGISTER_COMPONENT(SpotLightComponent);
	REGISTER_COMPONENT(SphereComponent);
}

EntityFactory::~EntityFactory()
{
	destroyEntities();
}

Ref<Component> EntityFactory::createComponent(const String& name, const LuaVariable& componentData)
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

Ref<Component> EntityFactory::createHierarchyComponent()
{
	Ref<HierarchyComponent> component(new HierarchyComponent());
	System::RegisterComponent(component.get());

	return component;
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

Ref<Entity> EntityFactory::createEntity(LuaTextResourceFile* actorLuaDescription)
{
	LuaInterpreter::GetSingleton()->loadExecuteScript(actorLuaDescription);

	LuaVariable entityDescription = LuaInterpreter::GetSingleton()->getGlobal("Entity");
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
	LuaVariable name = entityDescription["name"];
	entity.reset(new Entity(getNextID(), name.isNil() ? "Entity" : name));

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

	Ref<Component> hierarchyComponent = createHierarchyComponent();
	entity->addComponent(hierarchyComponent);
	hierarchyComponent->setOwner(entity);

	if (!entity->setupComponents())
	{
		ERR("Entity was not setup properly: " + std::to_string(entity->m_ID));
	}

	m_Entities.push_back(entity);
	return entity;
}

Ref<Entity> EntityFactory::createRootEntity()
{
	Ref<Entity> root;
	root.reset(new Entity(getNextID(), "Root"));
	
	Ref<RootHierarchyComponent> rootComponent(new RootHierarchyComponent());
	root->addComponent(rootComponent);
	rootComponent->setOwner(root);

	System::RegisterComponent(rootComponent.get());

	m_Entities.push_back(root);
	return root;
}

void EntityFactory::addDefaultComponent(Ref<Entity> entity, String componentName)
{
	entity->addComponent(createDefaultComponent(componentName));
	entity->setupComponents();
}

void EntityFactory::addComponent(Ref<Entity> entity, Ref<Component> component)
{
	entity->addComponent(component);
	component->setOwner(entity);
}

void EntityFactory::destroyEntities()
{
	for (auto& entity : m_Entities)
	{
		if (entity)
		{
			entity->destroy();
			WARN("Destroyed entity: " + entity->getName());
		}
		else
		{
			WARN("Found nullptr while browsing entities for destruction. Skipped during shutdown");
		}
	}
}
