
.. _program_listing_file_rootex_framework_components_debug_component.h:

Program Listing for File debug_component.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_debug_component.h>` (``rootex\framework\components\debug_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   
   class DebugComponent : public Component
   {
       static Component* Create(const JSON::json& componentData);
   
       friend class ECSFactory;
   
   public:
       static const ComponentID s_ID = (ComponentID)ComponentIDs::DebugComponent;
   
       virtual const char* getName() const override { return "DebugComponent"; }
       ComponentID getComponentID() const { return s_ID; }
       virtual JSON::json getJSON() const override;
   };
