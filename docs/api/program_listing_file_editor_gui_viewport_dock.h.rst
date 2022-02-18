
.. _program_listing_file_editor_gui_viewport_dock.h:

Program Listing for File viewport_dock.h
========================================

|exhale_lsh| :ref:`Return to documentation for file <file_editor_gui_viewport_dock.h>` (``editor\gui\viewport_dock.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "script/interpreter.h"
   #include "scene.h"
   
   #include "vendor/ImGUI/imgui.h"
   #include "vendor/ImGUI/imgui_impl_dx11.h"
   #include "vendor/ImGUI/imgui_impl_win32.h"
   
   class ViewportDock
   {
   public:
       struct ViewportDockSettings
       {
           bool m_IsActive = true;
           bool m_IsClosed;
           float m_AspectRatio;
       };
   
   private:
       Scene* m_EditorCamera;
       Scene* m_EditorGrid;
       ViewportDockSettings m_ViewportDockSettings;
       Matrix m_ApplyCameraMatrix;
   
       bool m_IsCameraMoving = false;
   
       float m_EditorCameraPitch = 0.0f;
       float m_EditorCameraYaw = 0.0f;
       float m_EditorCameraSpeed = 50.0f;
       float m_EditorCameraSensitivity = 300.0f;
       float m_EditorCameraRotationNormalizer = 1000.0f;
       float m_EditorCameraSmoothness = 0.9f;
   
   public:
       ViewportDock(const JSON::json& viewportJSON);
       ViewportDock(ViewportDock&) = delete;
       ~ViewportDock() = default;
   
       void draw(float deltaMilliseconds);
   
       ViewportDockSettings& getSettings() { return m_ViewportDockSettings; }
       void setActive(bool enabled) { m_ViewportDockSettings.m_IsActive = enabled; }
   };
