
.. _program_listing_file_rootex_framework_systems_pause_system.h:

Program Listing for File pause_system.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_pause_system.h>` (``rootex\framework\systems\pause_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "framework/scene.h"
   #include "framework/scene_loader.h"
   #include "framework/system.h"
   
   class PauseSystem : public System
   {
       PauseSystem();
       PauseSystem(PauseSystem&) = delete;
   
   private:
       bool m_IsPausingEnabled;
   
   public:
       static PauseSystem* GetSingleton();
   
       bool& getIsPausingEnabled() { return m_IsPausingEnabled; }
       void setIsPausingEnabled(bool pausing) { m_IsPausingEnabled = pausing; }
   
       void update(float deltaMilliseconds) override;
   };
