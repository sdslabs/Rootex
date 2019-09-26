#pragma once
#include <Windows.h>
#include <core/renderer/d3d11graphics.h>
#include <optional>
class RootexWindow
{
private:
	WNDCLASSEX windowClass = { 0 };
	LPCSTR className;
	HINSTANCE hInstance;
	HWND windowHandler;
	RootexGraphics* m_rootexGraphics = nullptr;

public:

	RootexWindow(int xOffset, int yOffset, int width, int height);
	RootexWindow(const RootexWindow&) = delete;
	RootexWindow& operator=(const RootexWindow&) = delete;  

	~RootexWindow();

	int gameLoop();
	static LRESULT CALLBACK customWindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam);
	RootexGraphics* getGraphics();
	std::optional<int> RootexWindow::ProcessMessages();
};