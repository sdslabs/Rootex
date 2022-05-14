
.. _program_listing_file_rootex_framework_entity.h:

Program Listing for File entity.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_entity.h>` (``rootex\framework\entity.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "script/interpreter.h"
   #include "event_manager.h"
   
   class Component;
   class Scene;
   class Script;
   
   typedef unsigned int ComponentID;
   typedef unsigned int SceneID;
   
   class Entity
   {
       EventBinder<Entity> m_Binder;
   
   protected:
       Scene* m_Scene;
       HashMap<ComponentID, Component*> m_Components;
       Ref<Script> m_Script;
   
   public:
       Entity(Scene* scene);
       Entity(Entity&) = delete;
       Entity(Entity&&) = delete;
       Entity& operator=(Entity&&) = delete;
       ~Entity();
   
       bool onAllComponentsAdded();
       bool onAllEntitiesAdded();
   
       void registerComponent(Component* component);
       bool addDefaultComponent(const String& componentName);
       bool addComponent(const String& componentName, const JSON::json& componentData);
       bool removeComponent(ComponentID toRemoveComponentID, bool hardRemove = false);
       bool hasComponent(ComponentID componentID);
   
       void clear();
   
       void destroy();
   
       Scene* getScene() const { return m_Scene; }
   
       template <class ComponentType = Component>
       ComponentType* getComponent();
   
       template <class ComponentType = Component>
       ComponentType* getComponentFromID(ComponentID ID);
   
       JSON::json getJSON() const;
       const String& getName() const;
       const SceneID getID() const;
       const String& getFullName() const;
       const HashMap<ComponentID, Component*>& getAllComponents() const;
   
       void bind(const Event::Type& event, const sol::function& function);
       bool call(const String& function, const Vector<Variant>& args);
       void evaluateScriptOverrides();
       bool setScript(const String& path);
       bool setScriptJSON(const JSON::json& script);
       Script* getScript() const { return m_Script.get(); }
   
       void draw();
   };
   
   template <class ComponentType>
   inline ComponentType* Entity::getComponent()
   {
       return getComponentFromID<ComponentType>(ComponentType::s_ID);
   }
   
   template <class ComponentType>
   inline ComponentType* Entity::getComponentFromID(ComponentID ID)
   {
       auto findIt = m_Components.find(ID);
       if (findIt != m_Components.end())
       {
           Component* baseTypeComponent = findIt->second;
           return dynamic_cast<ComponentType*>(baseTypeComponent);
       }
   
       return nullptr;
   }
