
.. _program_listing_file_rootex_core_renderer_dxgi_debug_interface.h:

Program Listing for File dxgi_debug_interface.h
===============================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_renderer_dxgi_debug_interface.h>` (``rootex\core\renderer\dxgi_debug_interface.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <dxgidebug.h>
   #include <vector>
   
   #include "common.h"
   
   class DxgiDebugInterface
   {
       unsigned long long next = 0u;
       Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_DxgiInfoQueue;
   
       DxgiDebugInterface();
       DxgiDebugInterface(const DxgiDebugInterface&) = delete;
       DxgiDebugInterface& operator=(const DxgiDebugInterface&) = delete;
       ~DxgiDebugInterface();
   
   public:
       static DxgiDebugInterface* GetSingleton();
   
       void set() noexcept;
       void getMessages(String file, String func) const;
   };
   
   #define GFX_ERR_CHECK(hr)                      \
       DxgiDebugInterface::GetSingleton()->set(); \
       if (FAILED(hr))                            \
       DxgiDebugInterface::GetSingleton()->getMessages(__FILE__, __func__)
