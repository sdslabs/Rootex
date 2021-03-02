
.. _program_listing_file_rootex_framework_component.h:

Program Listing for File component.h
====================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_component.h>` (``rootex\framework\component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "script/interpreter.h"
   #include "components/component_ids.h"
   
   typedef unsigned int ComponentID;
   class Component;
   
   class Dependable
   {
   public:
       virtual ComponentID getID() const = 0;
       virtual bool isValid() const = 0;
       virtual void setComponent(Component* component) = 0;
   };
   
   template <class ComponentType, bool isSoft>
   class Dependency : public Dependable
   {
       ComponentType* m_Component = nullptr;
   
   public:
       Dependency(Component* dependedBy)
       {
           dependedBy->registerDependency(this);
       }
       Dependency(Dependency&&) = delete;
       Dependency(const Dependency&) = delete;
       ~Dependency() = default;
   
       ComponentType*& getComponent() { return m_Component; }
       void setComponent(Component* component) override { m_Component = (ComponentType*)component; };
   
       ComponentID getID() const override { return ComponentType::s_ID; }
   
       bool isValid() const override { return isSoft || m_Component; }
   };
   
   #ifndef DEPENDS_ON
   #define DEPENDS_ON(ComponentType)                                                     \
   protected:                                                                            \
       ComponentType const* get##ComponentType() { return m_##ComponentType; }           \
       ComponentType*& m_##ComponentType = m_DependencyOn##ComponentType.getComponent(); \
       Dependency<ComponentType, false> m_DependencyOn##ComponentType
   #endif
   
   #ifndef SOFT_DEPENDS_ON
   #define SOFT_DEPENDS_ON(ComponentType)                                                \
   protected:                                                                            \
       ComponentType const* get##ComponentType() { return m_##ComponentType; }           \
       ComponentType*& m_##ComponentType = m_DependencyOn##ComponentType.getComponent(); \
       Dependency<ComponentType, true> m_DependencyOn##ComponentType
   #endif
   
   class Component
   {
       Vector<Dependable*> m_Dependencies;
   
       bool setup();
   
       friend class ECSFactory;
       friend class Entity;
   
   protected:
       Entity* m_Owner;
   
   public:
       Component();
       virtual ~Component();
   
       void registerDependency(Dependable* dependable) { m_Dependencies.push_back(dependable); }
       const Vector<Dependable*>& getDependencies() const { return m_Dependencies; }
   
       bool resolveDependencies();
       virtual bool setupData();
       virtual bool setupEntities();
       virtual void onRemove();
   
       virtual void onTrigger();
   
       Entity* getOwner() const;
       virtual ComponentID getComponentID() const = 0;
       virtual const char* getName() const = 0;
       virtual JSON::json getJSON() const;
   
       virtual void draw();
   };
