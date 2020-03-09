#pragma once

#include <optional>
#include <windows.h>

#include "common/common.h"

class Window
{
protected:
	int m_Width;
	int m_Height;

	WNDCLASSEX m_WindowClass = { 0 };
	LPCSTR m_ClassName;
	HINSTANCE m_AppInstance;
	HWND m_WindowHandle;
	
	static LRESULT CALLBACK WindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Window(int xOffset, int yOffset, int width, int height, const String& title, bool isEditor, bool MSAA);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window() = default;

	void show();
	std::optional<int> processMessages();
	void applyDefaultViewport();
	void swapBuffers();

	void clearCurrentTarget();
	void clearUnboundTarget();
	void setWindowTitle(String title);

	int getWidth() const;
	int getHeight() const;
	HWND getWindowHandle();
};
