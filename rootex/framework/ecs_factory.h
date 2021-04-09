#pragma once

#include "common/common.h"
#include "core/resource_files/text_resource_file.h"

#include "scene.h"
#include "entity.h"
#include "component.h"

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

#define MAX_COMPONENT_ARRAY_SIZE 1000

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
	Vector<T> m_Instances;

public:
	ComponentSet()
	{
		m_Instances.reserve(MAX_COMPONENT_ARRAY_SIZE); // To disallow resize if max size is detected later
	}

	Vector<T>& getAll() { return m_Instances; }

	bool addComponent(Entity& owner, const JSON::json& componentData, bool checks) override
	{
		if (m_Instances.size() == MAX_COMPONENT_ARRAY_SIZE)
		{
			ERR("Component set for " + T::s_Name + " is full. Reduce component count or increase max size");
		}

		if (!owner.hasComponent(T::s_ID))
		{
			m_Instances.emplace_back(owner, componentData);
			T& instance = m_Instances.back();

			owner.registerComponent(&instance);

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
			m_Instances.erase(findIt);
			return true;
		}
		return false;
	}

	const String& getName() const override { return T::s_Name; };
	const String& getCategory() const override { return T::s_Category; };
	const ComponentID& getID() const override { return T::s_ID; };
};

#define REGISTER_COMPONENT(Type)                                                              \
	static inline ComponentSet<Type> s_SetOf##Type;                                           \
                                                                                              \
	static bool Add##Type(Entity& owner, const JSON::json& componentData, bool checks = true) \
	{                                                                                         \
		return s_SetOf##Type.addComponent(owner, componentData, checks);                      \
	}                                                                                         \
                                                                                              \
	static bool AddDefault##Type(Entity& owner, bool checks = true)                           \
	{                                                                                         \
		return s_SetOf##Type.addDefaultComponent(owner, checks);                              \
	}                                                                                         \
                                                                                              \
	static bool Remove##Type(Entity& entity)                                                  \
	{                                                                                         \
		return s_SetOf##Type.removeComponent(entity);                                         \
	}                                                                                         \
	static Vector<Type>& GetAll##Type()                                                       \
	{                                                                                         \
		return s_SetOf##Type.getAll();                                                        \
	}

class ECSFactory
{
public:
	static inline HashMap<String, BaseComponentSet*> s_ComponentSets;

	static void Initialize();
	static void FillEntity(Entity& entity, const JSON::json& entityJSON);
	static void FillEntityFromFile(Entity& entity, TextResourceFile* textResourceFile);
	static void FillRootEntity(Entity& root);

	static void CopyEntity(Entity& entity, Entity& copyTarget);
	static String GetComponentNameByID(ComponentID componentID);
	static ComponentID GetComponentIDByName(const String& componentName);

	static bool AddComponent(Entity& entity, ComponentID componentID, const JSON::json& componentData, bool checks = true);
	static bool AddDefaultComponent(Entity& entity, ComponentID componentID, bool checks = true);
	static bool RemoveComponent(Entity& entity, ComponentID componentID);

	REGISTER_COMPONENT(PlayerController);
	REGISTER_COMPONENT(CameraComponent);
	REGISTER_COMPONENT(TransformComponent);
	REGISTER_COMPONENT(TransformAnimationComponent);
	REGISTER_COMPONENT(AudioListenerComponent);
	REGISTER_COMPONENT(MusicComponent);
	REGISTER_COMPONENT(ShortMusicComponent);
	REGISTER_COMPONENT(BoxColliderComponent);
	REGISTER_COMPONENT(CapsuleColliderComponent);
	REGISTER_COMPONENT(SphereColliderComponent);
	REGISTER_COMPONENT(StaticMeshColliderComponent);
	REGISTER_COMPONENT(TriggerComponent);
	REGISTER_COMPONENT(ModelComponent);
	REGISTER_COMPONENT(AnimatedModelComponent);
	REGISTER_COMPONENT(GridModelComponent);
	REGISTER_COMPONENT(PointLightComponent);
	REGISTER_COMPONENT(SpotLightComponent);
	REGISTER_COMPONENT(DirectionalLightComponent);
	REGISTER_COMPONENT(StaticPointLightComponent);
	REGISTER_COMPONENT(TextUIComponent);
	REGISTER_COMPONENT(UIComponent);
	REGISTER_COMPONENT(FogComponent);
	REGISTER_COMPONENT(SkyComponent);
	REGISTER_COMPONENT(CPUParticlesComponent);
	REGISTER_COMPONENT(ParticleEffectComponent);
};
