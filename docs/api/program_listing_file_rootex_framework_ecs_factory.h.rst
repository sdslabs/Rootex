
.. _program_listing_file_rootex_framework_ecs_factory.h:

Program Listing for File ecs_factory.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_ecs_factory.h>` (``rootex\framework\ecs_factory.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "core/resource_files/text_resource_file.h"
   
   #include "entity.h"
   #include "component.h"
   
   typedef Component* (*ComponentCreator)(const JSON::json& componentDescription);
   typedef Vector<Tuple<ComponentID, String, ComponentCreator>> ComponentDatabase;
   typedef HashMap<ComponentID, Vector<Component*>> ComponentInstanceDatabase;
   
   #define REGISTER_COMPONENT(ComponentType) ECSFactory::RegisterComponent<ComponentType>(#ComponentType)
   
   class ECSFactory
   {
       static inline ComponentDatabase s_ComponentCreators;
       static inline ComponentInstanceDatabase s_ComponentInstances;
   
   public:
       template <class T>
       static Vector<Component*>& GetComponents();
   
       static void Initialize();
   
       static bool AddComponent(Entity* entity, Ptr<Component>& component);
       static Ptr<Component> CreateComponent(const String& componentName, const JSON::json& componentData);
       static Ptr<Component> CreateDefaultComponent(const String& componentName);
   
       static Ptr<Entity> CreateEntity(Scene* scene, const JSON::json& entityJSON);
       static Ptr<Entity> CreateEntityFromFile(Scene* scene, TextResourceFile* textResourceFile);
       static Ptr<Entity> CreateEmptyEntity(Scene* scene);
       static Ptr<Entity> CreateRootEntity(Scene* scene);
       static Ptr<Entity> CopyEntity(Scene* scene, Entity& entity);
   
       static const ComponentDatabase& GetComponentDatabase() { return s_ComponentCreators; }
       template <class T>
       static void RegisterComponent(const String& name);
       static void RegisterComponentInstance(Component* component);
       static void DeregisterComponentInstance(Component* component);
   };
   
   template <class T>
   inline Vector<Component*>& ECSFactory::GetComponents()
   {
       return s_ComponentInstances[T::s_ID];
   }
   
   template <class T>
   inline void ECSFactory::RegisterComponent(const String& name)
   {
       s_ComponentCreators.push_back({ T::s_ID, name, T::Create });
   }
