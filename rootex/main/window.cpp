#include "main/window.h"

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

void Window::swapBuffers()
{
	RenderingDevice::GetSingleton()->swapBuffers();
}

int Window::gameLoop()
{
	while (true)
	{
		if (const std::optional<int> ecode = processMessages())
			return *ecode;
		//m_RootexGraphics->drawTest();
	}
}

void Window::clear()
{
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
	
	DXGI_SWAP_CHAIN_DESC descriptor = { 0 };
	descriptor.BufferDesc.Width = 0;
	descriptor.BufferDesc.Height = 0;
	descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	descriptor.BufferDesc.RefreshRate.Numerator = 0;
	descriptor.BufferDesc.RefreshRate.Denominator = 0;
	descriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	descriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	descriptor.SampleDesc.Count = 1;
	descriptor.SampleDesc.Quality = 0;
	descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descriptor.BufferCount = 1;
	descriptor.OutputWindow = m_WindowHandle;
	descriptor.Windowed = TRUE;
	descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	descriptor.Flags = 0;

	RenderingDevice::GetSingleton()->initialize(m_WindowHandle, width, height);
}
