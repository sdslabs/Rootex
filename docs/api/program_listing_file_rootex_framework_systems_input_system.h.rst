
.. _program_listing_file_rootex_framework_systems_input_system.h:

Program Listing for File input_system.h
=======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_input_system.h>` (``rootex\framework\systems\input_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   #include "event_manager.h"
   
   class InputSystem : public System
   {
       EventBinder<InputSystem> m_Binder;
   
       InputSystem();
       InputSystem(InputSystem&) = delete;
       ~InputSystem() = default;
   
       Variant windowResized(const Event* event);
   
   public:
       static InputSystem* GetSingleton();
   
       void loadSchemes(const HashMap<String, InputScheme>& schemes);
       void addScheme(const String& name, const InputScheme& scheme);
       void pushScheme(const String& name);
       void popScheme();
       void flushSchemes();
   
       bool initialize(const JSON::json& systemData) override;
       void setConfig(const SceneSettings& sceneSettings) override;
       void update(float deltaMilliseconds) override;
   };
