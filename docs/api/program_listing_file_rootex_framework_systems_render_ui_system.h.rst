
.. _program_listing_file_rootex_framework_systems_render_ui_system.h:

Program Listing for File render_ui_system.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_render_ui_system.h>` (``rootex\framework\systems\render_ui_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   
   class RenderUISystem : public System
   {
       Vector<Matrix> m_UITransformationStack;
   
       RenderUISystem();
       RenderUISystem(RenderUISystem&) = delete;
       virtual ~RenderUISystem() = default;
   
   public:
       static RenderUISystem* GetSingleton();
   
       void update(float deltaMilliseconds);
   
       void pushUIMatrix(const Matrix& transform);
       void popUIMatrix();
   
       Matrix getTopUIMatrix();
   };
