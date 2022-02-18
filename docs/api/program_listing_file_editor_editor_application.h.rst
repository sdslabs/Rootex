
.. _program_listing_file_editor_editor_application.h:

Program Listing for File editor_application.h
=============================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_editor_application.h>` (``editor\editor_application.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "rootex/app/application.h"
   
   class EditorApplication : public Application
   {
       static EditorApplication* s_Instance;
       static void SetSingleton(EditorApplication* app);
   
       unsigned int m_AutoSaveDurationS = 300.0f;
       TimePoint m_PointAtLast10Second;
       FrameTimer m_FrameTimer;
   
   public:
       static EditorApplication* GetSingleton();
   
       EditorApplication();
       EditorApplication(EditorApplication&) = delete;
       ~EditorApplication();
   
       void setGameMode(bool enabled);
   
       virtual void process(float deltaMilliseconds) override;
   };
