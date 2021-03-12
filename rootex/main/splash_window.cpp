#include "splash_window.h"

SplashWindow::SplashWindow(const String& title, const String& icon, const String& image, int width, int height)
{
	WNDCLASSEX windowClass = { 0 };
	LPCSTR className = title.c_str();
	HINSTANCE hInstance = GetModuleHandle(0);
	HANDLE iconRes = LoadImage(0, icon.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbWndExtra = 0;
	windowClass.cbClsExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = (HICON)iconRes;
	windowClass.hIconSm = (HICON)iconRes;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszClassName = className;
	RegisterClassEx(&windowClass);

	int x = (OS::GetDisplayWidth() - width) / 2;
	int y = (OS::GetDisplayHeight() - height) / 2;

	m_SplashWindow = CreateWindowEx(
	    WS_EX_TOPMOST,
	    className,
	    title.c_str(),
	    WS_POPUP,
	    x,
	    y,
	    x + width,
	    y + height,
	    nullptr,
	    nullptr,
	    hInstance,
	    nullptr);
	ShowWindow(m_SplashWindow, true);

	RECT rect;
	GetClientRect(m_SplashWindow, &rect);
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	HDC hdc = GetDC(m_SplashWindow);
	HBRUSH brush = CreatePatternBrush((HBITMAP)LoadImage(NULL, image.c_str(), IMAGE_BITMAP, width, height, LR_DEFAULTSIZE | LR_LOADFROMFILE));
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);
	ReleaseDC(m_SplashWindow, hdc);
}

SplashWindow::~SplashWindow()
{
	DestroyWindow(m_SplashWindow);
}
