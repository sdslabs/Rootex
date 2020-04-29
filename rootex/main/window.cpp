#include "main/window.h"

#include "core/event_manager.h"
#include "input/input_manager.h"
#include "renderer/rendering_device.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void Window::show()
{
	ShowWindow(m_WindowHandle, SW_SHOW);
}

std::optional<int> Window::processMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) //non-blocking message retrieval
	{
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

void Window::clearCurrentTarget()
{
#ifdef ROOTEX_EDITOR
	RenderingDevice::GetSingleton()->clearCurrentRenderTarget(0.15f, 0.15f, 0.15f);
#else
#ifdef DEBUG
	RenderingDevice::GetSingleton()->clearCurrentRenderTarget(0.3f, 0.7f, 0.3f);
#else
	RenderingDevice::GetSingleton()->clearCurrentRenderTarget(0.0f, 0.0f, 0.0f);
#endif // DEBUG
#endif // ROOTEX_EDITOR
}

void Window::clearUnboundTarget()
{
#ifdef ROOTEX_EDITOR
	RenderingDevice::GetSingleton()->clearUnboundRenderTarget(0.15f, 0.15f, 0.15f);
#else
#ifdef DEBUG
	RenderingDevice::GetSingleton()->clearUnboundRenderTarget(0.3f, 0.7f, 0.3f);
#else
	RenderingDevice::GetSingleton()->clearUnboundRenderTarget(0.0f, 0.0f, 0.0f);
#endif // DEBUG
#endif // ROOTEX_EDITOR
}

Variant Window::toggleFullScreen(const Event* event)
{
	m_IsFullScreen = !m_IsFullScreen;
	RenderingDevice::GetSingleton()->setScreenState(m_IsFullScreen);
	return true;
}

void Window::setWindowTitle(String title)
{
	SetWindowText(m_WindowHandle, title.c_str());
}

int Window::getWidth() const
{
	return m_Width;
}

int Window::getHeight() const
{
	return m_Height;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Window::WindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef ROOTEX_EDITOR
	if (ImGui_ImplWin32_WndProcHandler(windowHandler, msg, wParam, lParam))
	{
		return true;
	}
#endif // ROOTEX_EDITOR
	switch (msg)
	{
	case WM_CLOSE:
		EventManager::GetSingleton()->call("QuitWindowRequest", "QuitWindowRequest", 0);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	InputManager::GetSingleton()->forwardMessage({ windowHandler, msg, wParam, lParam });

	return DefWindowProc(windowHandler, msg, wParam, lParam);
}

Window::Window(int xOffset, int yOffset, int width, int height, const String& title, bool isEditor, bool MSAA, bool fullScreen)
    : m_Width(width)
    , m_Height(height)
{
	BIND_EVENT_MEMBER_FUNCTION("QuitWindowRequest", Window::quitWindow);
	BIND_EVENT_MEMBER_FUNCTION("QuitEditorWindow", Window::quitEditorWindow);
	BIND_EVENT_MEMBER_FUNCTION("WindowToggleFullScreen", Window::toggleFullScreen);
	BIND_EVENT_MEMBER_FUNCTION("WindowGetScreenState", Window::getScreenState);
	WNDCLASSEX windowClass = { 0 };
	LPCSTR className = title.c_str();
	HINSTANCE hInstance = GetModuleHandle(0);
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = WindowsProc;
	windowClass.cbWndExtra = 0;
	windowClass.cbClsExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = nullptr;
	RegisterClassEx(&windowClass);
	m_IsEditorWindow = isEditor;

	if (isEditor)
	{
		m_WindowHandle = CreateWindowEx(
		    0, className,
		    title.c_str(),
		    WS_CAPTION | WS_BORDER | WS_MAXIMIZE | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU,
		    xOffset, yOffset, width, height,
		    nullptr, nullptr,
		    hInstance, nullptr);

		RECT clientRect;
		GetClientRect(m_WindowHandle, &clientRect);
		int rWidth = clientRect.right - clientRect.left;
		int rHeight = clientRect.bottom - clientRect.top;
		RenderingDevice::GetSingleton()->initialize(
		    m_WindowHandle,
		    rWidth,
		    rHeight,
		    MSAA);
	}
	else
	{
		m_WindowHandle = CreateWindowEx(
		    0, className,
		    title.c_str(),
		    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		    xOffset, yOffset, width, height,
		    nullptr, nullptr,
		    hInstance, nullptr);

		RECT clientRect;
		GetClientRect(m_WindowHandle, &clientRect);
		int rWidth = clientRect.right - clientRect.left;
		int rHeight = clientRect.bottom - clientRect.top;
		ClipCursor(&clientRect);
		RenderingDevice::GetSingleton()->initialize(
		    m_WindowHandle,
		    rWidth,
		    rHeight,
		    MSAA);

		RenderingDevice::GetSingleton()->setBackBufferRenderTarget();
	}
	applyDefaultViewport();
	m_IsFullScreen = false;
	if (fullScreen)
	{
		EventManager::GetSingleton()->deferredCall("WindowToggleFullScreen", "WindowToggleFullScreen", 0);
	}
}

Variant Window::quitWindow(const Event* event)
{
	if (m_IsEditorWindow)
	{
		EventManager::GetSingleton()->call("EditorSaveBeforeQuit", "EditorSaveBeforeQuit", 0);
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

HWND Window::getWindowHandle()
{
	return m_WindowHandle;
}
