#pragma once

#include <optional>
#include <windows.h>
#include "core/event_manager.h"

#include "common/common.h"

/// Handles window creation.
class Window
{
protected:
	int m_Width;
	int m_Height;
	bool m_IsEditorWindow;
	bool m_IsFullScreen;

	WNDCLASSEX m_WindowClass = { 0 };
	LPCSTR m_ClassName;
	HINSTANCE m_AppInstance;
	HWND m_WindowHandle;
	
	/// Wraps DefWindowProc function.
	static LRESULT CALLBACK WindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam);
	Variant quitWindow(const Event* event);
	Variant quitEditorWindow(const Event* event);
	Variant windowResized(const Event* event);

public:
	Window(int xOffset, int yOffset, int width, int height, const String& title, bool isEditor, bool MSAA, bool fullScreen);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window() = default;
	void show();
	std::optional<int> processMessages();
	void applyDefaultViewport();
	void swapBuffers();
	/// Clips or blocks the cursor beyond the specified rectangle.
	void clipCursor(RECT clip);
	/// Reset cursor clips and allow free cursor movement.
	void resetClipCursor();
	void showCursor(bool enabled);
	void clearMain(const Color& color);
	void clearOffScreen(const Color& color);

	Variant toggleFullScreen(const Event* event);
	Variant getScreenState(const Event* event) { return m_IsFullScreen; };

	int getWidth() const;
	int getHeight() const;
	int getTitleBarHeight() const;
	HWND getWindowHandle();
	void setWindowTitle(String title);
	void setWindowSize(const Vector2& newSize);
};
