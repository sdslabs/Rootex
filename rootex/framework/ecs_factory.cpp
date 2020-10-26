#include "ecs_factory.h"

#include "system.h"

#include "scene.h"

#include "components/audio_listener_component.h"
#include "components/debug_component.h"
#include "components/hierarchy_component.h"
#include "components/music_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/script_component.h"
#include "components/short_music_component.h"
#include "components/test_component.h"
#include "components/transform_animation_component.h"
#include "components/transform_component.h"
#include "components/trigger_component.h"
#include "components/visual/camera_component.h"
#include "components/visual/cpu_particles_component.h"
#include "components/visual/directional_light_component.h"
#include "components/visual/fog_component.h"
#include "components/visual/grid_model_component.h"
#include "components/visual/model_component.h"
#include "components/visual/point_light_component.h"
#include "components/visual/static_point_light_component.h"
#include "components/visual/sky_component.h"
#include "components/visual/spot_light_component.h"
#include "components/visual/text_ui_component.h"
#include "components/visual/ui_component.h"
#include "systems/hierarchy_system.h"

bool ECSFactory::AddComponent(Ref<Entity>& entity, Ref<Component> component)
{
	if (entity->m_Components.find(component->getComponentID()) == entity->m_Components.end())
	{
		entity->m_Components[component->getComponentID()] = component;
		component->m_Owner = entity.get();
		entity->onAllComponentsAdded();
		return true;
	}

	WARN("Entity already has a " + component->getName() + ": " + entity->getFullName());
	return false;
}

Ref<Component> ECSFactory::CreateComponent(const String& componentName, const JSON::json& componentData)
{
	auto& findIt = s_ComponentCreators.end();
	for (auto& componentClass = s_ComponentCreators.begin(); componentClass != s_ComponentCreators.end(); componentClass++)
	{
		if (std::get<String>(*componentClass) == componentName)
		{
			findIt = componentClass;
		}
	}
	if (findIt != s_ComponentCreators.end())
	{
		ComponentCreator create = std::get<ComponentCreator>(*findIt);
		Ref<Component> component(create(componentData));

		System::RegisterComponent(component.get());

		return component;
	}
	ERR("Could not find component creator: " + componentName);
	return nullptr;
}

Ref<Component> ECSFactory::CreateDefaultComponent(const String& componentName)
{
	return CreateComponent(componentName, JSON::json::object());
}

Ref<Entity> ECSFactory::CreateEntity(Scene* scene, const JSON::json& entityJSON)
{
	if (entityJSON.empty())
	{
		return std::make_shared<Entity>(scene, scene->getID(), "Entity");
	}

	JSON::json componentJSON;
	if (!componentJSON.empty())
	{
		componentJSON = entityJSON["Components"];
	}

	Ref<Entity> entity(std::make_shared<Entity>(scene, scene->getID(), entityJSON["Entity"].value("name", String("Entity"))));

	for (auto&& [componentName, componentDescription] : componentJSON.items())
	{
		Ref<Component> componentObject = CreateComponent(componentName, componentDescription);
		if (componentObject)
		{
			entity->m_Components[componentObject->getComponentID()] = componentObject;
			componentObject->m_Owner = entity.get();
		}
	}

	if (!entity->onAllComponentsAdded())
	{
		ERR("Entity was not setup properly: " + std::to_string(entity->m_ID));
	}

	PRINT("Created entity: " + entity->getFullName());
	return entity;
}

Ref<Entity> ECSFactory::CreateEmptyEntity(Scene* scene)
{
	return CreateEntity(scene, {});
}

Ref<Entity> ECSFactory::CreateEntity(Scene* scene, TextResourceFile* textResourceFile)
{
	return CreateEntity(scene, JSON::json::parse(textResourceFile->getString()));
}

Ref<Entity> ECSFactory::CopyEntity(Scene* scene, Entity& entity)
{
	return CreateEntity(scene, entity.getJSON());
}

void ECSFactory::Initialize()
{
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
	REGISTER_COMPONENT(StaticPointLightComponent);
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

Ref<Entity> ECSFactory::CreateRootEntity(Scene* scene)
{
	Ref<Entity> root = CreateEmptyEntity(scene);
	root->setName("Root");

	{
		Ref<Component> rootTransformComponent = CreateDefaultComponent("TransformComponent");
		AddComponent(root, rootTransformComponent);
	}
	{
		Ref<ModelComponent> rootModelComponent = std::dynamic_pointer_cast<ModelComponent>(CreateDefaultComponent("ModelComponent"));
		rootModelComponent->setIsVisible(false);
		AddComponent(root, rootModelComponent);
	}
	{
		Ref<CameraComponent> rootCameraComponent = std::dynamic_pointer_cast<CameraComponent>(CreateDefaultComponent("CameraComponent"));
		AddComponent(root, rootCameraComponent);
	}
	{
		Ref<AudioListenerComponent> rootListenerComponent = std::dynamic_pointer_cast<AudioListenerComponent>(CreateDefaultComponent("AudioListenerComponent"));
		AddComponent(root, rootListenerComponent);
	}
	{
		Ref<SkyComponent> rootSkyComponent = std::dynamic_pointer_cast<SkyComponent>(CreateDefaultComponent("SkyComponent"));
		AddComponent(root, rootSkyComponent);
	}

	return root;
}
