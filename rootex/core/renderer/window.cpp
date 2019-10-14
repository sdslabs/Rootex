#include <core/renderer/window.h>

std::optional<int> RootexWindow::ProcessMessages()
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

int RootexWindow::gameLoop()
{
	while (true)
	{
		if (const std::optional<int> ecode = RootexWindow::ProcessMessages())
			return *ecode;
		this->getGraphics()->EndFrame();
	}
}

LRESULT CALLBACK RootexWindow::customWindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	}
	return DefWindowProc(windowHandler, msg, wParam, lParam);
}

RootexGraphics* RootexWindow::getGraphics()
{
	return m_rootexGraphics;
}

RootexWindow::RootexWindow(int xOffset, int yOffset, int width, int height)
{
	WNDCLASSEX windowClass = { 0 };
	LPCSTR className = "Game";
	HINSTANCE hInstance = GetModuleHandle(0);
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = customWindowsProc;
	windowClass.cbWndExtra = 0;
	windowClass.cbClsExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszClassName = "Game";
	windowClass.hIconSm = nullptr;
	RegisterClassEx(&windowClass);
	HWND windowHandler = CreateWindowEx(
	    0, className,
	    "OH NOOOO",
	    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
	    100, 200, 640, 480,
	    nullptr, nullptr,
	    hInstance, nullptr);
	ShowWindow(windowHandler, SW_SHOW);
	m_rootexGraphics = new RootexGraphics(windowHandler);
}

RootexWindow::~RootexWindow()
{
	delete m_rootexGraphics;
}
