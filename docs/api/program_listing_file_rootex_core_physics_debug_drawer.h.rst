
.. _program_listing_file_rootex_core_physics_debug_drawer.h:

Program Listing for File debug_drawer.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_physics_debug_drawer.h>` (``rootex\core\physics\debug_drawer.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "core/renderer/material_library.h"
   #include "Bullet3D/src/LinearMath/btIDebugDraw.h"
   
   class Material;
   class BasicMaterial;
   
   class DebugDrawer : public btIDebugDraw
   {
       Ref<BasicMaterial> m_BasicMaterial;
   
   public:
       DebugDrawer();
       DebugDrawer(DebugDrawer&) = delete;
       ~DebugDrawer() = default;
   
       Material* getMaterial() { return m_BasicMaterial.get(); }
   
       virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
       virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
       virtual void reportErrorWarning(const char* warningString) override;
       virtual void draw3dText(const btVector3& location, const char* textString) override;
       virtual void setDebugMode(int debugMode) override;
       virtual int getDebugMode() const override;
   };
