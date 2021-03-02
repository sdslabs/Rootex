
.. _program_listing_file_rootex_core_ui_input_interface.h:

Program Listing for File input_interface.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_core_ui_input_interface.h>` (``rootex\core\ui\input_interface.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "common/common.h"
   #include "event.h"
   
   #undef interface
   #include "RmlUi/Core.h"
   #define interface __STRUCT__
   
   class InputInterface
   {
   public:
       static bool s_IsMouseOver;
       static bool s_IsEnabled;
       static float s_ScaleX;
       static float s_ScaleY;
       static int s_Left;
       static int s_Right;
       static int s_Top;
       static int s_Bottom;
   
       static bool Initialise();
       static void Shutdown();
   
       static void ProcessWindowsEvent(UINT message, WPARAM wParam, LPARAM lParam);
   
       static void SetContext(Rml::Context* context);
       static Rml::Character GetCharacterCode(Rml::Input::KeyIdentifier keyIdentifier, int keyModifier_state);
   
   protected:
       static Rml::Context* s_Context;
   };
