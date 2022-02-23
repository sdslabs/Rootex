
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
   #include "ecs_factory.h"
   
   class Component;
   
   #define DECLARE_COMPONENT(Type)                                                             \
       namespace ECSFactory                                                                    \
       {                                                                                       \
           bool Add##Type(Entity& owner, const JSON::json& componentData, bool checks = true); \
           bool AddDefault##Type(Entity& owner, bool checks = true);                           \
           bool Remove##Type(Entity& entity);                                                  \
           Vector<Type>& GetAll##Type();                                                       \
       }
   
   #define DEFINE_COMPONENT(Type)                                                              \
       bool ECSFactory::Add##Type(Entity& owner, const JSON::json& componentData, bool checks) \
       {                                                                                       \
           return s_ComponentSets[Type::s_Name]->addComponent(owner, componentData, checks);   \
       }                                                                                       \
       bool ECSFactory::AddDefault##Type(Entity& owner, bool checks)                           \
       {                                                                                       \
           return s_ComponentSets[Type::s_Name]->addDefaultComponent(owner, checks);           \
       }                                                                                       \
       bool ECSFactory::Remove##Type(Entity& entity)                                           \
       {                                                                                       \
           return s_ComponentSets[Type::s_Name]->removeComponent(entity);                      \
       }                                                                                       \
       Vector<Type>& ECSFactory::GetAll##Type()                                                \
       {                                                                                       \
           return ((ComponentSet<Type>*)(s_ComponentSets[Type::s_Name].get()))->getAll();      \
       }
   
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
       ~Dependency() = default;
   
       ComponentType* getComponent() { return m_Component; }
       const ComponentType* getComponent() const { return m_Component; }
       void setComponent(Component* component) override { m_Component = (ComponentType*)component; };
   
       ComponentID getID() const override { return ComponentType::s_ID; }
   
       bool isValid() const override { return isSoft || m_Component; }
   };
   
   #ifndef DEPENDENCY
   #define DEPENDENCY(ComponentType, mode)                                                                      \
   public:                                                                                                      \
       Dependency<ComponentType, mode> m_DependencyOn##ComponentType;                                           \
       ComponentType* get##ComponentType() { return m_DependencyOn##ComponentType.getComponent(); }             \
       const ComponentType* get##ComponentType() const { return m_DependencyOn##ComponentType.getComponent(); } \
                                                                                                                \
   private:
   #endif
   
   #ifndef DEPENDS_ON
   #define DEPENDS_ON(ComponentType) DEPENDENCY(ComponentType, false)
   #endif
   
   #ifndef SOFT_DEPENDS_ON
   #define SOFT_DEPENDS_ON(ComponentType) DEPENDENCY(ComponentType, true)
   #endif
   
   #define COMPONENT(ComponentType, category)                                           \
   public:                                                                              \
       static inline const String s_Name = #ComponentType;                              \
       static inline const ComponentID s_ID = (ComponentID)ComponentIDs::ComponentType; \
       static inline const String s_Category = category;                                \
                                                                                        \
       const char* getName() const override { return #ComponentType; }                  \
       ComponentID getComponentID() const override { return s_ID; }                     \
                                                                                        \
   private:
   
   class Component
   {
       Vector<Dependable*> m_Dependencies;
   
       bool setup();
   
       friend class Entity;
   
   protected:
       struct Category
       {
           static inline const String General = "General";
           static inline const String Audio = "Audio";
           static inline const String Game = "Game";
           static inline const String Physics = "Physics";
           static inline const String Model = "Model";
           static inline const String Effect = "Effect";
           static inline const String Light = "Light";
           static inline const String UI = "UI";
       };
       Entity* m_Owner;
   
   public:
       Component(Entity& owner);
       virtual ~Component() = default;
   
       void registerDependency(Dependable* dependable) { m_Dependencies.push_back(dependable); }
       const Vector<Dependable*>& getDependencies() const { return m_Dependencies; }
   
       bool resolveDependencies();
       virtual bool setupData() { return true; }
       virtual bool setupEntities() { return true; }
       virtual void onRemove() { }
   
       Entity& getOwner() { return *m_Owner; }
       virtual ComponentID getComponentID() const = 0;
       virtual const char* getName() const = 0;
       virtual JSON::json getJSON() const;
   
       virtual void draw();
   };
