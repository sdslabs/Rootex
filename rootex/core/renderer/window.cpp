#include <core/renderer/window.h>

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
		//this->getGraphics()->EndFrame();

		//The color change effect ->
		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
		float seconds = elapsed_seconds.count();
		float r = (sin(seconds) + 1.0) * 0.5;
		float g = (sin(seconds * 0.3) + 1.0) * 0.5;
		float b = (sin(seconds * 0.7) + 1.0) * 0.5;
		this->getGraphics()->ClearBuffer(r, g, b);

		this->getGraphics()->DrawTestTriangle(seconds);
		this->getGraphics()->EndFrame();
	}
}

LRESULT CALLBACK GameWindow::customWindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	}
	return DefWindowProc(windowHandler, msg, wParam, lParam);
}

RootexGraphics* GameWindow::getGraphics()
{
	return m_RootexGraphics;
}

GameWindow::GameWindow(int xOffset, int yOffset, int width, int height)
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
	m_RootexGraphics = new RootexGraphics(windowHandler);

	start = std::chrono::system_clock::now();
}

GameWindow::~GameWindow()
{
	delete m_RootexGraphics;
}
