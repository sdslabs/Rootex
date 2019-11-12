#pragma once

#include <Windows.h>
#include <optional>

#include "core/renderer/d3d11graphics.h"
#include <chrono>

class GameWindow
{
private:
	WNDCLASSEX m_WindowClass = { 0 };
	LPCSTR m_ClassName;
	HINSTANCE m_AppInstance;
	HWND m_WindowHandler;
	RootexGraphics* m_RootexGraphics = nullptr;

	std::chrono::time_point<std::chrono::system_clock> start;

public:

	GameWindow(int xOffset, int yOffset, int width, int height);
	GameWindow(const GameWindow&) = delete;
	GameWindow& operator=(const GameWindow&) = delete;  

	~GameWindow();

	int gameLoop();
	static LRESULT CALLBACK customWindowsProc(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam);
	RootexGraphics* getGraphics();
	std::optional<int> GameWindow::ProcessMessages();
};
