
.. _program_listing_file_rootex_core_physics_bullet_conversions.h:

Program Listing for File bullet_conversions.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_physics_bullet_conversions.h>` (``rootex\core\physics\bullet_conversions.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   
   #include "LinearMath/btTransform.h"
   #include "LinearMath/btVector3.h"
   
   btTransform MatTobtTransform(Matrix const& mat);
   Matrix BtTransformToMat(btTransform const& trans);
   btVector3 VecTobtVector3(Vector3 const& vec3);
   Vector3 BtVector3ToVec(btVector3 const& btvec);
