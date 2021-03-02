
.. _program_listing_file_rootex_main_window.h:

Program Listing for File window.h
=================================

|exhale_lsh| :ref:`Return to documentation for file <file_rootex_main_window.h>` (``rootex\main\window.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <optional>
   #include <windows.h>
   #include "core/event_manager.h"
   
   #include "common/common.h"
   
   class Window
   {
   protected:
       int m_Width;
       int m_Height;
       bool m_IsEditorWindow;
       bool m_IsFullscreen;
   
       WNDCLASSEX m_WindowClass = { 0 };
       LPCSTR m_ClassName;
       HINSTANCE m_AppInstance;
       HWND m_WindowHandle;
   
       static LRESULT CALLBACK WindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam);
       Variant quitWindow(const Event* event);
       Variant quitEditorWindow(const Event* event);
       Variant windowResized(const Event* event);
   
   public:
       Window(int xOffset, int yOffset, int width, int height, const String& title, bool isEditor, bool fullScreen, const String& icon);
       Window(const Window&) = delete;
       Window& operator=(const Window&) = delete;
       ~Window() = default;
       void show();
       std::optional<int> processMessages();
       void applyDefaultViewport();
       void swapBuffers();
       void clipCursor(RECT clip);
       void resetClipCursor();
       void showCursor(bool enabled);
       void clearMain(const Color& color);
       void clearOffScreen(const Color& color);
   
       Variant toggleFullscreen(const Event* event);
       Variant getScreenState(const Event* event) { return m_IsFullscreen; };
   
       int getWidth() const;
       int getHeight() const;
       int getTitleBarHeight() const;
       HWND getWindowHandle();
       void setWindowTitle(String title);
       void setWindowSize(const Vector2& newSize);
   };
