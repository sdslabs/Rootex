
.. _program_listing_file_rootex_core_renderer_spot_light.h:

Program Listing for File spot_light.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_spot_light.h>` (``rootex\core\renderer\spot_light.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   struct SpotLight
   {
       float attConst;
       float attLin;
       float attQuad;
       float range;
       float diffuseIntensity;
       Color diffuseColor;
       Color ambientColor;
       float spot;
       float angleRange;
   };
