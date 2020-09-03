#include "entity_factory.h"

#include "core/event_manager.h"

#include "component.h"
#include "entity.h"
#include "system.h"

#include "components/debug_component.h"
#include "components/hierarchy_component.h"
#include "components/music_component.h"
#include "components/audio_listener_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/script_component.h"
#include "components/short_music_component.h"
#include "components/test_component.h"
#include "components/transform_animation_component.h"
#include "components/transform_component.h"
#include "components/trigger_component.h"
#include "components/visual/fog_component.h"
#include "components/visual/camera_component.h"
#include "components/visual/sky_component.h"
#include "components/visual/cpu_particles_component.h"
#include "components/visual/directional_light_component.h"
#include "components/visual/grid_model_component.h"
#include "components/visual/model_component.h"
#include "components/visual/point_light_component.h"
#include "components/visual/spot_light_component.h"
#include "components/visual/text_ui_component.h"
#include "components/visual/ui_component.h"
#include "systems/hierarchy_system.h"

#define REGISTER_COMPONENT(ComponentClass)                                                            \
	m_ComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::Create }); \
	m_DefaultComponentCreators.push_back({ ComponentClass::s_ID, #ComponentClass, ComponentClass::CreateDefault })

EntityID EntityFactory::s_CurrentID = ROOT_ENTITY_ID;
EntityID EntityFactory::s_CurrentEditorID = -ROOT_ENTITY_ID;

void EntityFactory::RegisterAPI(sol::state& rootex)
{
	sol::usertype<EntityFactory> entityFactory = rootex.new_usertype<EntityFactory>("EntityFactory");
	entityFactory["Create"] = [](TextResourceFile* t) { return EntityFactory::GetSingleton()->createEntity(t); };
	entityFactory["Find"] = [](EntityID e) { return EntityFactory::GetSingleton()->findEntity(e); };
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

EntityID EntityFactory::getNextEditorID()
{
	return --s_CurrentEditorID;
}

EntityFactory::EntityFactory()
{
	BIND_EVENT_MEMBER_FUNCTION("DeleteEntity", deleteEntityEvent);

	REGISTER_COMPONENT(TestComponent);
	REGISTER_COMPONENT(DebugComponent);
	REGISTER_COMPONENT(CameraComponent);
	REGISTER_COMPONENT(GridModelComponent);
	REGISTER_COMPONENT(ModelComponent);
	REGISTER_COMPONENT(FogComponent);
	REGISTER_COMPONENT(TextUIComponent);
	REGISTER_COMPONENT(SkyComponent);
	REGISTER_COMPONENT(TransformComponent);
	REGISTER_COMPONENT(TransformAnimationComponent);
	REGISTER_COMPONENT(PointLightComponent);
	REGISTER_COMPONENT(DirectionalLightComponent);
	REGISTER_COMPONENT(SpotLightComponent);
	REGISTER_COMPONENT(SphereColliderComponent);
	REGISTER_COMPONENT(BoxColliderComponent);
	REGISTER_COMPONENT(HierarchyComponent);
	REGISTER_COMPONENT(ScriptComponent);
	REGISTER_COMPONENT(AudioListenerComponent);
	REGISTER_COMPONENT(MusicComponent);
	REGISTER_COMPONENT(ShortMusicComponent);
	REGISTER_COMPONENT(CPUParticlesComponent);
	REGISTER_COMPONENT(TriggerComponent);
	REGISTER_COMPONENT(UIComponent);
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

Ref<Entity> EntityFactory::createEntity(TextResourceFile* textResourceFile, bool isEditorOnly)
{
	return createEntity(JSON::json::parse(textResourceFile->getString()), textResourceFile->getPath().generic_string(), isEditorOnly);
}

Ref<Entity> EntityFactory::createEntity(const JSON::json& entityJSON, const String& filePath, bool isEditorOnly)
{
	if (entityJSON.is_null())
	{
		ERR("Entity not found:" + filePath);
		return nullptr;
	}

	JSON::json componentJSON = entityJSON["Components"];
	if (componentJSON.is_null())
	{
		ERR("Components not found while creating Entity:" + filePath);
		return nullptr;
	}

	Ref<Entity> entity;
	JSON::json name = entityJSON["Entity"]["name"];

	EntityID newID = 0;
	if (isEditorOnly)
	{
		newID = getNextEditorID();
	}
	else
	{
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

	entity->setEditorOnly(isEditorOnly);

	m_Entities[entity->m_ID] = entity;

	PRINT("Created entity: " + entity->getFullName());

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

void EntityFactory::setupLiveEntities()
{
	for (auto& entity : m_Entities)
	{
		entity.second->setupEntities();
	}
}

Ref<Entity> EntityFactory::createRootEntity()
{
	Ref<Entity> root;
	root.reset(new Entity(ROOT_ENTITY_ID, "Root"));

	{
		Ref<HierarchyComponent> rootComponent(new HierarchyComponent(INVALID_ID, {}));
		System::RegisterComponent(rootComponent.get());
		addComponent(root, rootComponent);
	}
	{
		Ref<Component> rootTransformComponent = createDefaultComponent("TransformComponent");
		addComponent(root, rootTransformComponent);
	}
	{
		Ref<ModelComponent> rootModelComponent = std::dynamic_pointer_cast<ModelComponent>(createDefaultComponent("ModelComponent"));
		rootModelComponent->setIsVisible(false);
		addComponent(root, rootModelComponent);
	}
	{
		Ref<CameraComponent> rootCameraComponent = std::dynamic_pointer_cast<CameraComponent>(createDefaultComponent("CameraComponent"));
		addComponent(root, rootCameraComponent);
	}
	{
		Ref<AudioListenerComponent> rootListenerComponent = std::dynamic_pointer_cast<AudioListenerComponent>(createDefaultComponent("AudioListenerComponent"));
		addComponent(root, rootListenerComponent);
	}

	m_Entities[root->m_ID] = root;
	return root;
}

Variant EntityFactory::deleteEntityEvent(const Event* event)
{
	deleteEntity(Extract(Ref<Entity>, event->getData()));
	return true;
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
			if (entity.second->getID() == ROOT_ENTITY_ID || entity.second->getID() == INVALID_ID || entity.second->isEditorOnly())
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
		deleteEntity(entity);
	}

	if (!saveRoot && m_Entities[ROOT_ENTITY_ID])
	{
		deleteEntity(m_Entities[ROOT_ENTITY_ID]);
	}
}

void EntityFactory::deleteEntity(Ref<Entity> entity)
{
	entity->destroy();
	m_Entities.erase(entity->getID());
	entity.reset();
}

bool EntityFactory::saveEntityAsClass(Ref<Entity> entity)
{
	if (OS::IsExists("game/assets/classes/" + entity->getName()))
	{
		WARN("Class with name \"" + entity->getName() + "\" already exists. New Class not created.");
		return false;
	}
	else
	{
		OS::CreateDirectoryName("game/assets/classes/" + entity->getName());
		saveEntityAsClassRecursively(entity, "game/assets/classes/" + entity->getName() + "/");
	}

	return true;
}

String EntityFactory::saveEntityAsClassRecursively(Ref<Entity> entity, const String& path)
{
	Ref<HierarchyComponent> hierarchyComponent = entity->getComponent<HierarchyComponent>();
	JSON::json& entityJSON = entity->getJSON();
	Vector<String> children;
	for (EntityID child : hierarchyComponent->m_ChildrenIDs)
	{
		children.push_back(saveEntityAsClassRecursively(EntityFactory::GetSingleton()->findEntity(child), path));
	}
	entityJSON["Components"]["HierarchyComponent"]["children"] = children;
	entityJSON["Components"]["HierarchyComponent"]["parent"] = ROOT_ENTITY_ID;
	entityJSON["Entity"].erase("ID");
	InputOutputFileStream file = OS::CreateFileName(path + entity->getName() + ".entity.json");
	file << entityJSON.dump(4) << std::endl;
	file.close();
	return path + entity->getName() + ".entity.json";
}

Ref<Entity> EntityFactory::createEntityFromClass(TextResourceFile* entityFile)
{
	return createEntityFromClass(JSON::json::parse(entityFile->getString()));
}

Ref<Entity> EntityFactory::createEntityFromClass(const JSON::json& entityJSON)
{
	Ref<Entity> createdEntity = createEntityHierarchyFromClass(entityJSON);
	fixParentID(createdEntity, ROOT_ENTITY_ID);
	return createdEntity;
}

Ref<Entity> EntityFactory::createEntityHierarchyFromClass(JSON::json entityJSON)
{
	Vector<EntityID> ids;

	for (String path : entityJSON["Components"]["HierarchyComponent"]["children"])
	{
		if (OS::IsFile(path))
		{
			JSON::json entityClass;
			TextResourceFile* classFile = ResourceLoader::CreateTextResourceFile(path);
			entityClass = JSON::json::parse(classFile->getString());
			ids.push_back(createEntityHierarchyFromClass(entityClass)->getID());
		}
	}

	entityJSON["Components"]["HierarchyComponent"]["children"] = ids;
	Ref<Entity> newEntity = createEntity(entityJSON, "Error in creating" + entityJSON["Entity"]["Name"].dump());
	if (!newEntity)
	{
		WARN("Could not create entity: " + entityJSON["Entity"]["Name"].dump());
	}
	return newEntity;
}

void EntityFactory::fixParentID(Ref<Entity> entity, EntityID id)
{
	Ref<HierarchyComponent> hierarchyComponent = entity->getComponent<HierarchyComponent>();
	hierarchyComponent->m_ParentID = id;
	entity->setupEntities();
	for (EntityID childID : hierarchyComponent->m_ChildrenIDs)
	{
		fixParentID(findEntity(childID), entity->getID());
	}
}
