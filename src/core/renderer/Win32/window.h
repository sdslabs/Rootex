#pragma once
#include <Windows.h>

class RootexWindow
{
private:
	WNDCLASSEX windowClass = { 0 };
	LPCSTR className;
	HINSTANCE hInstance;
	HWND windowHandler;
public:

	RootexWindow(int xOffset, int yOffset, int width, int height);
	RootexWindow(const RootexWindow&) = delete;
	RootexWindow& operator=(const RootexWindow&) = delete;  

	~RootexWindow();

	int gameLoop();
	static LRESULT CALLBACK customWindowsProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};