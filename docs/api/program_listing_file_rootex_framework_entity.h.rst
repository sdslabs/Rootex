
.. _program_listing_file_rootex_framework_entity.h:

Program Listing for File entity.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_entity.h>` (``rootex\framework\entity.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "script/interpreter.h"
   #include "event.h"
   
   class Component;
   class Scene;
   class Script;
   
   typedef unsigned int ComponentID;
   typedef int EntityID;
   
   class Entity
   {
   protected:
       Scene* m_Scene;
       HashMap<ComponentID, Ptr<Component>> m_Components;
       Ptr<Script> m_Script;
   
       friend class ECSFactory;
   
   public:
       Entity(Scene* scene, const JSON::json& script = {});
       Entity(const Entity&) = delete;
       ~Entity();
   
       bool onAllComponentsAdded();
       bool onAllEntitiesAdded();
   
       bool addDefaultComponent(const String& componentName);
       bool addComponent(const String& componentName, const JSON::json& componentData);
       bool removeComponent(ComponentID toRemoveComponentID, bool hardRemove = false);
       bool hasComponent(ComponentID componentID);
   
       void destroy();
   
       Scene* getScene() const { return m_Scene; }
   
       template <class ComponentType = Component>
       ComponentType* getComponent();
   
       template <class ComponentType = Component>
       ComponentType* getComponentFromID(ComponentID ID);
   
       JSON::json getJSON() const;
       const String& getName() const;
       const String& getFullName() const;
       const HashMap<ComponentID, Ptr<Component>>& getAllComponents() const;
   
       bool call(const String& function, const Vector<Variant>& args);
       void evaluateScriptOverrides();
       bool setScript(const String& path);
       Script* getScript() const { return m_Script.get(); }
   
       void draw();
   };
   
   template <class ComponentType>
   inline ComponentType* Entity::getComponent()
   {
       auto findIt = m_Components.find(ComponentType::s_ID);
       if (findIt != m_Components.end())
       {
           Ptr<Component>& baseTypeComponent = findIt->second;
           return dynamic_cast<ComponentType*>(baseTypeComponent.get());
       }
   
       return nullptr;
   }
   
   template <class ComponentType>
   inline ComponentType* Entity::getComponentFromID(ComponentID ID)
   {
       auto findIt = m_Components.find(ID);
       if (findIt != m_Components.end())
       {
           Ptr<Component>& baseTypeComponent = findIt->second;
           return dynamic_cast<ComponentType*>(baseTypeComponent.get());
       }
   
       return nullptr;
   }
