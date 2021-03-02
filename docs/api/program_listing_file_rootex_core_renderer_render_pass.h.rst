
.. _program_listing_file_rootex_core_renderer_render_pass.h:

Program Listing for File render_pass.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_render_pass.h>` (``rootex\core\renderer\render_pass.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   enum class RenderPass : int
   {
       Basic = 1 << 0,
       Editor = 1 << 1,
       Alpha = 1 << 2
   };
