
.. _program_listing_file_rootex_app_application.h:

Program Listing for File application.h
======================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_app_application.h>` (``rootex\app\application.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "main/splash_window.h"
   #include "main/window.h"
   #include "core/event_manager.h"
   #include "os/timer.h"
   #include "os/thread.h"
   #include "application_settings.h"
   
   class Application
   {
       static Application* s_Singleton;
   
   protected:
       Timer m_ApplicationTimer;
       FrameTimer m_FrameTimer;
       ThreadPool m_ThreadPool;
       float m_DeltaMultiplier = 1.0f;
       String m_ApplicationTitle;
       int m_CurrentSaveSlot;
       JSON::json m_CurrentSaveData;
   
       Ptr<SplashWindow> m_SplashWindow;
       Ptr<Window> m_Window;
       Ptr<ApplicationSettings> m_ApplicationSettings;
   
       String getSaveSlotPath(int slot);
   
   public:
       static Application* GetSingleton();
   
       Application(const String& appTitle, const String& settingsFile);
       Application(Application&) = delete;
       virtual ~Application();
   
       void run();
       virtual void process(float deltaMilliseconds);
       void end();
   
       void createSaveSlot(int slot);
       bool loadSave(int slot);
       JSON::json& getSaveData();
       bool saveSlot();
   
       const String& getAppTitle() const { return m_ApplicationTitle; };
       const Timer& getAppTimer() const { return m_ApplicationTimer; };
       ThreadPool& getThreadPool() { return m_ThreadPool; };
       const FrameTimer& getAppFrameTimer() const { return m_FrameTimer; }
       Window* getWindow() { return m_Window.get(); };
       ApplicationSettings* getSettings() { return m_ApplicationSettings.get(); }
       Vector<FilePath> getLibrariesPaths();
   
       void destroySplashWindow();
   
       float* getDeltaMultiplierPtr() { return &m_DeltaMultiplier; }
       float getDeltaMultiplier() const { return m_DeltaMultiplier; }
       void setDeltaMultiplier(float gain) { m_DeltaMultiplier = gain; }
       void resetDeltaMultiplier() { setDeltaMultiplier(1.0f); }
   };
   
   extern Ref<Application> CreateRootexApplication();
