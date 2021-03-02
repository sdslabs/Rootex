
.. _program_listing_file_rootex_core_renderer_directional_light.h:

Program Listing for File directional_light.h
============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_directional_light.h>` (``rootex\core\renderer\directional_light.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   struct DirectionalLight
   {
       float diffuseIntensity;
       Color diffuseColor;
       Color ambientColor;
   };
