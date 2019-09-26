#include "window.h"

int RootexWindow::gameLoop()
{
	MSG msg;
	BOOL msgRet;
	while ((msgRet = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msgRet == -1)
		return -1;
	else
		return msg.wParam;
	return 0;
}

LRESULT CALLBACK RootexWindow::customWindowsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
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
}

RootexWindow::~RootexWindow()
{
}
