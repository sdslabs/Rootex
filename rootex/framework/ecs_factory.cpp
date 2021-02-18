#include "ecs_factory.h"

#include "system.h"

#include "scene.h"

#include "components/audio_listener_component.h"
#include "components/debug_component.h"
#include "components/music_component.h"
#include "components/physics/box_collider_component.h"
#include "components/physics/sphere_collider_component.h"
#include "components/physics/capsule_collider_component.h"
#include "components/physics/static_mesh_collider_component.h"
#include "components/short_music_component.h"
#include "components/test_component.h"
#include "components/transform_animation_component.h"
#include "components/transform_component.h"
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
#include "components/visual/animated_model_component.h"

void ECSFactory::RegisterComponentInstance(Component* component)
{
	s_ComponentInstances[component->getComponentID()].push_back(component);
}

void ECSFactory::DeregisterComponentInstance(Component* component)
{
	Vector<Component*>& components = s_ComponentInstances[component->getComponentID()];

	auto findIt = std::find(components.begin(), components.end(), component);
	if (findIt != components.end())
	{
		components.erase(findIt);
	}
	else
	{
		ERR("Found an unregistered component queued for deregisteration: " + component->getName());
	}
}

bool ECSFactory::AddComponent(Entity* entity, Ptr<Component>& component)
{
	if (entity->m_Components.find(component->getComponentID()) == entity->m_Components.end())
	{
		component->m_Owner = entity;
		ComponentID id = component->getComponentID();
		entity->m_Components[id] = std::move(component);
		if (!entity->onAllComponentsAdded())
		{
			if (entity->hasComponent(id))
			{
				entity->removeComponent(id, true);
			}
			return false;
		}
		return true;
	}

	WARN("Entity already has a " + component->getName() + ": " + entity->getFullName());
	return false;
}

Ptr<Component> ECSFactory::CreateComponent(const String& componentName, const JSON::json& componentData)
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
		Ptr<Component> component(create(componentData));

		RegisterComponentInstance(component.get());

		return component;
	}
	ERR("Could not find component creator: " + componentName);
	return nullptr;
}

Ptr<Component> ECSFactory::CreateDefaultComponent(const String& componentName)
{
	return CreateComponent(componentName, JSON::json::object());
}

Ptr<Entity> ECSFactory::CreateEntity(Scene* scene, const JSON::json& entityJSON)
{
	if (entityJSON.empty())
	{
		return std::make_unique<Entity>(scene);
	}

	JSON::json componentJSON;
	if (entityJSON.contains("components"))
	{
		componentJSON = entityJSON["components"];
	}
	JSON::json scriptJSON = {};
	if (entityJSON.contains("Entity") && entityJSON["Entity"].contains("script"))
	{
		scriptJSON = entityJSON["Entity"]["script"];
	}
	Ptr<Entity> entity(std::make_unique<Entity>(scene, scriptJSON));

	for (auto&& [componentName, componentDescription] : componentJSON.items())
	{
		Ptr<Component>& componentObject = CreateComponent(componentName, componentDescription);
		if (componentObject)
		{
			componentObject->m_Owner = entity.get();
			entity->m_Components[componentObject->getComponentID()] = std::move(componentObject);
		}
	}

	if (!entity->onAllComponentsAdded())
	{
		ERR("Entity scene was not setup properly: " + std::to_string(scene->getID()));
	}

	PRINT("Created entity: " + entity->getFullName());
	return entity;
}

Ptr<Entity> ECSFactory::CreateEmptyEntity(Scene* scene)
{
	return CreateEntity(scene, {});
}

Ptr<Entity> ECSFactory::CreateEntityFromFile(Scene* scene, TextResourceFile* textResourceFile)
{
	return CreateEntity(scene, JSON::json::parse(textResourceFile->getString()));
}

Ptr<Entity> ECSFactory::CopyEntity(Scene* scene, Entity& entity)
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
	REGISTER_COMPONENT(CapsuleColliderComponent);
	REGISTER_COMPONENT(StaticMeshColliderComponent);
	REGISTER_COMPONENT(AudioListenerComponent);
	REGISTER_COMPONENT(MusicComponent);
	REGISTER_COMPONENT(ShortMusicComponent);
	REGISTER_COMPONENT(CPUParticlesComponent);
	REGISTER_COMPONENT(UIComponent);
	REGISTER_COMPONENT(AnimatedModelComponent);
}

Ptr<Entity> ECSFactory::CreateRootEntity(Scene* scene)
{
	Ptr<Entity>& root = CreateEmptyEntity(scene);
	{
		Ptr<Component>& rootTransformComponent = CreateDefaultComponent("TransformComponent");
		AddComponent(root.get(), rootTransformComponent);
	}
	{
		Ptr<Component>& model = CreateDefaultComponent("ModelComponent");
		ModelComponent* rootModelComponent = dynamic_cast<ModelComponent*>(model.get());
		rootModelComponent->setIsVisible(false);
		AddComponent(root.get(), model);
	}
	{
		Ptr<Component>& camera = CreateDefaultComponent("CameraComponent");
		CameraComponent* rootCameraComponent = dynamic_cast<CameraComponent*>(camera.get());
		AddComponent(root.get(), camera);
	}
	{
		Ptr<Component>& listener = CreateDefaultComponent("AudioListenerComponent");
		AudioListenerComponent* rootListenerComponent = dynamic_cast<AudioListenerComponent*>(listener.get());
		AddComponent(root.get(), listener);
	}
	{
		Ptr<Component>& sky = CreateDefaultComponent("SkyComponent");
		SkyComponent* rootSkyComponent = dynamic_cast<SkyComponent*>(sky.get());
		AddComponent(root.get(), sky);
	}

	return std::move(root);
}
