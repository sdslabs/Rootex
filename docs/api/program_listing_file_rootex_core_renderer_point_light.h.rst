
.. _program_listing_file_rootex_core_renderer_point_light.h:

Program Listing for File point_light.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_point_light.h>` (``rootex\core\renderer\point_light.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   
   struct PointLight
   {
       float attConst;
       float attLin;
       float attQuad;
       float range;
       float diffuseIntensity;
       Color diffuseColor;
       Color ambientColor;
   };
