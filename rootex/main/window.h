#pragma once

#include <optional>
#include <windows.h>

#include "common/common.h"
#include "core/event_manager.h"

class Window
{
protected:
	int m_Width;
	int m_Height;
	bool m_FullScreen;

	WNDCLASSEX m_WindowClass = { 0 };
	LPCSTR m_ClassName;
	HINSTANCE m_AppInstance;
	HWND m_WindowHandle;
	
	static LRESULT CALLBACK WindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Window(int xOffset, int yOffset, int width, int height, const String& title, bool isEditor, bool MSAA, bool fullScreen);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window() = default;

	void show();
	std::optional<int> processMessages();
	void applyDefaultViewport();
	void swapBuffers();
	void clipCursor();

	void clearCurrentTarget();
	void clearUnboundTarget();

	Variant toggleFullScreen(const Event* event);
	Variant getScreenState(const Event* event) { return m_FullScreen; };

	int getWidth() const;
	int getHeight() const;
	HWND getWindowHandle();

	void setWindowTitle(String title);
};
