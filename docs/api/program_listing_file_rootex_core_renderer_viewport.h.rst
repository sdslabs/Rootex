
.. _program_listing_file_rootex_core_renderer_viewport.h:

Program Listing for File viewport.h
===================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_viewport.h>` (``rootex\core\renderer\viewport.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <d3d11.h>
   
   class Viewport
   {
       D3D11_VIEWPORT m_Viewport;
   
   public:
       Viewport(float width, float height, float minDepth, float maxDepth, float topLeftX, float topLeftY);
       Viewport(Viewport&) = delete;
       ~Viewport();
   
       const D3D11_VIEWPORT* getViewport() const { return &m_Viewport; }
   };
