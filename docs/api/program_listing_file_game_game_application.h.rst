
.. _program_listing_file_game_game_application.h:

Program Listing for File game_application.h
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file_game_game_application.h>` (``game\game_application.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "rootex/app/application.h"
   #include "framework/systems/script_system.h"
   
   class GameApplication : public Application
   {
       Optional<String> getSceneNameFromCommandLine(const char* s);
       Variant onExitEvent(const Event* event);
   
   public:
       GameApplication();
       GameApplication(GameApplication&) = delete;
       ~GameApplication() = default;
   };
