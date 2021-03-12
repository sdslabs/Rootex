#include "main/window.h"

#include "core/event_manager.h"
#include "core/ui/input_interface.h"
#include "input/input_manager.h"
#include "renderer/rendering_device.h"

#include "editor/editor_events.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"
#include "Tracy/Tracy.hpp"

void Window::show()
{
	ShowWindow(m_WindowHandle, SW_SHOW);
}

std::optional<int> Window::processMessages()
{
	ZoneScoped;

	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) //non-blocking message retrieval
	{
		ZoneNamedN(windowPeek, "Window Msg Peek", true);
		switch (msg.message)
		{
		case WM_QUIT:
			return (int)msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

void Window::applyDefaultViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = m_Width;
	vp.Height = m_Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	RenderingDevice::GetSingleton()->setViewport(&vp);
}

void Window::swapBuffers()
{
	ZoneScoped;
	RenderingDevice::GetSingleton()->swapBuffers();
}

void Window::clipCursor(RECT clip)
{
	ClipCursor(&clip);
}

void Window::resetClipCursor()
{
	ClipCursor(nullptr);
}

void Window::showCursor(bool enabled)
{
	ShowCursor(enabled);
}

void Window::clearMain(const Color& color)
{
	RenderingDevice::GetSingleton()->clearMainRT(color.x, color.y, color.z, color.w);
	RenderingDevice::GetSingleton()->clearDSV();
}

void Window::clearOffScreen(const Color& color)
{
	RenderingDevice::GetSingleton()->clearOffScreenRT(color.x, color.y, color.z, color.w);
	RenderingDevice::GetSingleton()->clearDSV();
}

Variant Window::toggleFullscreen(const Event* event)
{
	m_IsFullscreen = !m_IsFullscreen;
	RenderingDevice::GetSingleton()->setScreenState(m_IsFullscreen);
	return true;
}

void Window::setWindowTitle(String title)
{
	SetWindowText(m_WindowHandle, title.c_str());
}

void Window::setWindowSize(const Vector2& newSize)
{
	m_Width = newSize.x;
	m_Height = newSize.y;
}

int Window::getWidth() const
{
	return m_Width;
}

int Window::getHeight() const
{
	return m_Height;
}

int Window::getTitleBarHeight() const
{
	RECT clientRect;
	GetClientRect(m_WindowHandle, &clientRect);
	int clientHeight = clientRect.bottom - clientRect.top;
	return m_Height - clientHeight;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Window::WindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(windowHandler, msg, wParam, lParam))
	{
		return true;
	}
	switch (msg)
	{
	case WM_CLOSE:
		EventManager::GetSingleton()->call(RootexEvents::QuitWindowRequest);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
	{
		Vector2 size = { (float)LOWORD(lParam), (float)HIWORD(lParam) };
		if (size.x > 0.0f && size.y > 0.0f)
		{
			EventManager::GetSingleton()->call(RootexEvents::WindowResized, size);
		}
	}
	break;
	}

	InputInterface::ProcessWindowsEvent(msg, wParam, lParam);
	InputManager::GetSingleton()->forwardMessage({ windowHandler, msg, wParam, lParam });

	return DefWindowProc(windowHandler, msg, wParam, lParam);
}

Window::Window(int xOffset, int yOffset, int width, int height, const String& title, bool isEditor, bool fullScreen, const String& icon)
    : m_Width(width)
    , m_Height(height)
{
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::QuitWindowRequest, Window::quitWindow);
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::QuitEditorWindow, Window::quitEditorWindow);
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowToggleFullscreen, Window::toggleFullscreen);
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowGetScreenState, Window::getScreenState);
	BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, Window::windowResized);

	WNDCLASSEX windowClass = { 0 };
	LPCSTR className = title.c_str();
	HINSTANCE hInstance = GetModuleHandle(0);
	HANDLE iconRes = LoadImage(0, icon.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = WindowsProc;
	windowClass.cbWndExtra = 0;
	windowClass.cbClsExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = (HICON)iconRes;
	windowClass.hIconSm = (HICON)iconRes;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszClassName = className;
	RegisterClassEx(&windowClass);
	m_IsEditorWindow = isEditor;

	m_WindowHandle = CreateWindowEx(
	    0,
	    className,
	    title.c_str(),
	    WS_CAPTION | WS_BORDER | WS_MAXIMIZE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX,
	    xOffset,
	    yOffset,
	    width,
	    height,
	    nullptr,
	    nullptr,
	    hInstance,
	    nullptr);

	RECT clientRect;
	GetClientRect(m_WindowHandle, &clientRect);
	int rWidth = clientRect.right - clientRect.left;
	int rHeight = clientRect.bottom - clientRect.top;
	RenderingDevice::GetSingleton()->initialize(m_WindowHandle, rWidth, rHeight);

	applyDefaultViewport();
	m_IsFullscreen = false;
	if (fullScreen)
	{
		EventManager::GetSingleton()->deferredCall(RootexEvents::WindowToggleFullscreen);
	}
}

Variant Window::quitWindow(const Event* event)
{
	if (m_IsEditorWindow)
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorSaveBeforeQuit);
	}
	else
	{
		PostQuitMessage(0);
	}
	return true;
}

Variant Window::quitEditorWindow(const Event* event)
{
	DestroyWindow(getWindowHandle());
	return true;
}

Variant Window::windowResized(const Event* event)
{
	const Vector2& newSize = Extract<Vector2>(event->getData());
	setWindowSize(newSize);
	applyDefaultViewport();
	return true;
}

HWND Window::getWindowHandle()
{
	return m_WindowHandle;
}
