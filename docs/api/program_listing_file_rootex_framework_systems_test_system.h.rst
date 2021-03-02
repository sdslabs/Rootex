
.. _program_listing_file_rootex_framework_systems_test_system.h:

Program Listing for File test_system.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_systems_test_system.h>` (``rootex\framework\systems\test_system.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "system.h"
   
   class TestSystem : public System
   {
   public:
       TestSystem();
   
       void update(float deltaMilliseconds) override;
   };
