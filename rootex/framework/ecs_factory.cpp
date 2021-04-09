#include "ecs_factory.h"

#include "system.h"

#include "scene.h"

#define ASSIGN_COMPONENT_SET(ComponentType) s_ComponentSets[ComponentType::s_Name] = &s_SetOf##ComponentType

void ECSFactory::FillEntity(Entity& entity, const JSON::json& entityJSON)
{
	if (entityJSON.empty())
	{
		return;
	}

	JSON::json componentJSON;
	if (entityJSON.contains("components"))
	{
		componentJSON = entityJSON["components"];
	}

	entity.setScriptJSON(entityJSON.value("script", JSON::json::object()));

	for (auto&& [componentName, componentDescription] : componentJSON.items())
	{
		if (!s_ComponentSets[componentName]->addComponent(entity, componentDescription, false))
		{
			PRINT("Could not add " + componentName + " to " + entity.getName());
		}
	}

	if (!entity.onAllComponentsAdded())
	{
		ERR("Entity was not setup properly: " + std::to_string(entity.getID()));
	}

	PRINT("Filled entity: " + entity.getFullName());
}

void ECSFactory::FillEntityFromFile(Entity& entity, TextResourceFile* textResourceFile)
{
	FillEntity(entity, JSON::json::parse(textResourceFile->getString()));
}

void ECSFactory::CopyEntity(Entity& entity, Entity& copyTarget)
{
	FillEntity(entity, copyTarget.getJSON());
}

String ECSFactory::GetComponentNameByID(ComponentID componentID)
{
	for (auto& componentData : s_ComponentSets)
	{
		if (componentData.second->getID() == componentID)
		{
			return componentData.first;
		}
	}
	WARN("Could not find component name with ID: " + std::to_string(componentID));
	return "";
}

ComponentID ECSFactory::GetComponentIDByName(const String& componentName)
{
	return s_ComponentSets[componentName]->getID();
}

bool ECSFactory::RemoveComponent(Entity& entity, ComponentID componentID)
{
	return s_ComponentSets[GetComponentNameByID(componentID)]->removeComponent(entity);
}

void ECSFactory::FillRootEntity(Entity& root)
{
	AddDefaultTransformComponent(root);
	AddDefaultModelComponent(root);
	root.getComponent<ModelComponent>()->setVisibility(false);
	AddDefaultCameraComponent(root);
	AddDefaultAudioListenerComponent(root);
	AddDefaultSkyComponent(root);
}

void ECSFactory::Initialize()
{
	ASSIGN_COMPONENT_SET(PlayerController);

	ASSIGN_COMPONENT_SET(CameraComponent);
	ASSIGN_COMPONENT_SET(TransformComponent);
	ASSIGN_COMPONENT_SET(TransformAnimationComponent);

	ASSIGN_COMPONENT_SET(AudioListenerComponent);
	ASSIGN_COMPONENT_SET(MusicComponent);
	ASSIGN_COMPONENT_SET(ShortMusicComponent);

	ASSIGN_COMPONENT_SET(BoxColliderComponent);
	ASSIGN_COMPONENT_SET(CapsuleColliderComponent);
	ASSIGN_COMPONENT_SET(SphereColliderComponent);
	ASSIGN_COMPONENT_SET(StaticMeshColliderComponent);
	ASSIGN_COMPONENT_SET(TriggerComponent);

	ASSIGN_COMPONENT_SET(ModelComponent);
	ASSIGN_COMPONENT_SET(AnimatedModelComponent);
	ASSIGN_COMPONENT_SET(GridModelComponent);

	ASSIGN_COMPONENT_SET(PointLightComponent);
	ASSIGN_COMPONENT_SET(SpotLightComponent);
	ASSIGN_COMPONENT_SET(DirectionalLightComponent);
	ASSIGN_COMPONENT_SET(StaticPointLightComponent);

	ASSIGN_COMPONENT_SET(TextUIComponent);
	ASSIGN_COMPONENT_SET(UIComponent);

	ASSIGN_COMPONENT_SET(FogComponent);
	ASSIGN_COMPONENT_SET(SkyComponent);
	ASSIGN_COMPONENT_SET(CPUParticlesComponent);
	ASSIGN_COMPONENT_SET(ParticleEffectComponent);
}

bool ECSFactory::AddComponent(Entity& entity, ComponentID componentID, const JSON::json& componentData, bool checks)
{
	return s_ComponentSets[GetComponentNameByID(componentID)]->addComponent(entity, componentData, checks);
}

bool ECSFactory::AddDefaultComponent(Entity& entity, ComponentID componentID, bool checks)
{
	return s_ComponentSets[GetComponentNameByID(componentID)]->addDefaultComponent(entity, checks);
}
