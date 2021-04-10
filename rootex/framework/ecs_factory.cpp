#include "ecs_factory.h"

#include "system.h"

#include "scene.h"
#include "components/audio/audio_listener_component.h"
#include "components/audio/music_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/physics/capsule_collider_component.h"
#include "components/physics/static_mesh_collider_component.h"
#include "components/physics/trigger_component.h"
#include "components/audio/short_music_component.h"
#include "components/space/transform_animation_component.h"
#include "components/space/transform_component.h"
#include "components/visual/camera_component.h"
#include "components/visual/effect/cpu_particles_component.h"
#include "components/visual/light/directional_light_component.h"
#include "components/visual/effect/fog_component.h"
#include "components/visual/model/grid_model_component.h"
#include "components/visual/model/model_component.h"
#include "components/visual/light/point_light_component.h"
#include "components/visual/light/static_point_light_component.h"
#include "components/visual/effect/sky_component.h"
#include "components/visual/light/spot_light_component.h"
#include "components/visual/ui/text_ui_component.h"
#include "components/visual/ui/ui_component.h"
#include "components/visual/model/animated_model_component.h"
#include "components/visual/effect/particle_effect_component.h"
#include "components/game/player_controller.h"

HashMap<String, Ptr<BaseComponentSet>> ECSFactory::s_ComponentSets;

#define ASSIGN_COMPONENT_SET(ComponentType) s_ComponentSets[ComponentType::s_Name] = std::make_unique<ComponentSet<ComponentType>>()

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
	ECSFactory::AddDefaultTransformComponent(root);
	ECSFactory::AddDefaultModelComponent(root);
	root.getComponent<ModelComponent>()->setVisibility(false);
	ECSFactory::AddDefaultCameraComponent(root);
	ECSFactory::AddDefaultAudioListenerComponent(root);
	ECSFactory::AddDefaultSkyComponent(root);
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
