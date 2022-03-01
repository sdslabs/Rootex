
.. _program_listing_file_rootex_main_splash_window.h:

Program Listing for File splash_window.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_main_splash_window.h>` (``rootex\main\splash_window.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/types.h"
   
   class SplashWindow
   {
       HWND m_SplashWindow;
   
   public:
       SplashWindow(const String& title, const String& icon, const String& image, int width, int height);
       SplashWindow(const SplashWindow&) = delete;
       ~SplashWindow();
   };
