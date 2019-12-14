#include "core/renderer/directx/window.h"

std::optional<int> GameWindow::ProcessMessages()
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

int GameWindow::gameLoop()
{
	while (true)
	{
		if (const std::optional<int> ecode = GameWindow::ProcessMessages())
			return *ecode;
		m_GraphicsHandler->endFrame();
	}
}

LRESULT CALLBACK GameWindow::handleWindowsMessage(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE: // Same behaviour as WM_QUIT
	case WM_QUIT:
	{
		// TODO: Perform safe shutdown procedure. Close down drivers
		// TODO: Discuss if we need to track if they ever shut the game down
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	return DefWindowProc(windowHandler, msg, wParam, lParam);
}

RenderingDeviceD3D* GameWindow::getRenderingDevice()
{
	return m_GraphicsHandler.get();
}

GameWindow::GameWindow(int xOffset, int yOffset, int width, int height, const String& title)
{
	WNDCLASSEX windowClass = { 0 };
	LPCSTR className = "Game";
	HINSTANCE hInstance = GetModuleHandle(0);
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = handleWindowsMessage;
	windowClass.cbWndExtra = 0;
	windowClass.cbClsExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszClassName = "Game";
	windowClass.hIconSm = nullptr;
	RegisterClassEx(&windowClass);
	
	PANIC(title == "", "Window title was read empty, using empty title");
	HWND windowHandler = CreateWindowEx(
	    0, className,
	    title.c_str(),
	    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
	    xOffset, yOffset, width, height,
	    nullptr, nullptr,
	    hInstance, nullptr);

	ShowWindow(windowHandler, SW_SHOW);
	
	m_GraphicsHandler = std::make_unique<RenderingDeviceD3D>(windowHandler);
	PANIC(m_GraphicsHandler == nullptr, "Graphics could not be instantiated. Low memory.");
}

GameWindow::~GameWindow()
{
}
