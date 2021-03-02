
.. _program_listing_file_rootex_framework_components_test_component.h:

Program Listing for File test_component.h
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_test_component.h>` (``rootex\framework\components\test_component.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "component.h"
   
   class TestComponent : public Component
   {
       static Component* Create(const JSON::json& componentData);
   
       friend class ECSFactory;
   
   public:
       virtual const char* getName() const override { return "TestComponent"; }
       static const ComponentID s_ID = (ComponentID)ComponentIDs::TestComponent;
   
       ComponentID getComponentID() const override { return s_ID; }
       virtual JSON::json getJSON() const override;
   };
