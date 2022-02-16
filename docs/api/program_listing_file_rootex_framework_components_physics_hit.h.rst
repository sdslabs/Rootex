
.. _program_listing_file_rootex_framework_components_physics_hit.h:

Program Listing for File hit.h
==============================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_framework_components_physics_hit.h>` (``rootex\framework\components\physics\hit.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "entity.h"
   
   struct Hit
   {
       Entity* thisOne;
       Entity* thatOne;
   
       Hit::Hit(Entity* left, Entity* right)
           : thisOne(left)
           , thatOne(right)
       {
       }
       Hit(const Hit&) = delete;
       ~Hit() = default;
   };
