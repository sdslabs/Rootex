#include "main/window.h"

#include "input/input_manager.h"
#include "renderer/rendering_device.h"

std::optional<int> Window::processMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) //non-blocking message retrieval
	{
		if (msg.message == WM_QUIT)
		{
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

void Window::clear()
{
	ClipCursor(&m_Clip);
	SetCursorPos(m_Clip.left, m_Clip.top);
	RenderingDevice::GetSingleton()->clearBuffer(0.0f, 0.0f, 0.0f);
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

LRESULT CALLBACK Window::WindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	InputManager::GetSingleton()->forwardMessage({ windowHandler, msg, wParam, lParam });

	return DefWindowProc(windowHandler, msg, wParam, lParam);
}

Window::Window(int xOffset, int yOffset, int width, int height, const String& title)
    : m_Width(width)
    , m_Height(height)
{
	WNDCLASSEX windowClass = { 0 };
	LPCSTR className = "Game";
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
	windowClass.lpszClassName = "Game";
	windowClass.hIconSm = nullptr;
	RegisterClassEx(&windowClass);

	m_WindowHandle = CreateWindowEx(
	    0, className,
	    title.c_str(),
	    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
	    xOffset, yOffset, width, height,
	    nullptr, nullptr,
	    hInstance, nullptr);
	ShowWindow(m_WindowHandle, SW_SHOW);

	RenderingDevice::GetSingleton()->initialize(m_WindowHandle, width, height);
	applyDefaultViewport();

	GetWindowRect(m_WindowHandle, &m_Clip);

	// Modify the rect slightly, so the frame doesn't get clipped with
	m_Clip.left += 5;
	m_Clip.top += 30;
	m_Clip.right -= 5;
	m_Clip.bottom -= 5;

	ClipCursor(&m_Clip);
	ShowCursor(false);
}
