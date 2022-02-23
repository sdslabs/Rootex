
.. _program_listing_file_editor_gui_scene_dock.h:

Program Listing for File scene_dock.h
=====================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_scene_dock.h>` (``editor\gui\scene_dock.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "scene.h"
   
   class SceneDock
   {
       EventBinder<SceneDock> m_Binder;
   
   public:
       struct SceneDockSettings
       {
           bool m_IsActive = true;
       };
   
   private:
       SceneDockSettings m_SceneDockSettings;
       SceneID m_OpenedSceneID;
   
       void showSceneTree(Ptr<Scene>& scene);
       void openScene(Scene* scene);
   
       Variant selectOpenScene(const Event* event);
   
   public:
       SceneDock();
       SceneDock(const SceneDock&) = delete;
       ~SceneDock() = default;
   
       void draw(float deltaMilliseconds);
   
       void showEntities(Scene* scene);
   
       SceneDockSettings& getSettings() { return m_SceneDockSettings; }
       void setActive(bool enabled) { m_SceneDockSettings.m_IsActive = enabled; }
   };
