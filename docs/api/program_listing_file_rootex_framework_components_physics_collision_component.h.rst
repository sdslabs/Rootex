
.. _program_listing_file_rootex_framework_components_physics_collision_component.h:

Program Listing for File collision_component.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_collision_component.h>` (``rootex\framework\components\physics\collision_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   #include "component.h"
   #include "hit.h"
   
   #include "btBulletCollisionCommon.h"
   
   enum class CollisionMask : unsigned int
   {
       None = 0,
       Player = 1 << 0,
       Enemy = 1 << 1,
       Architecture = 1 << 2,
       TriggerVolume = 1 << 3,
       Other = 1 << 4,
       All = Player | Enemy | Architecture | TriggerVolume | Other
   };
   
   class CollisionComponent : public Component
   {
   protected:
       Ref<btCollisionObject> m_CollisionObject;
       unsigned int m_CollisionGroup;
       unsigned int m_CollisionMask;
   
       void detachCollisionObject();
       void attachCollisionObject();
   
   public:
       CollisionComponent(Entity& owner, int collisionGroup, int collisionMask);
       virtual ~CollisionComponent() = default;
   
       virtual void handleHit(Hit* h);
   
       void onRemove() override;
       JSON::json getJSON() const override;
       void draw() override;
       void displayCollisionLayers(unsigned int& collision);
   };
