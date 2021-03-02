
.. _program_listing_file_rootex_core_renderer_index_triangle_list.h:

Program Listing for File index_triangle_list.h
==============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_index_triangle_list.h>` (``rootex\core\renderer\index_triangle_list.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include <vector>
   
   template <typename T>
   struct IndexTriangleList
   {
       std::vector<T> vertices;
       std::vector<unsigned short> indices;
   };
