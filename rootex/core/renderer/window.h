#pragma once

#include <Windows.h>
#include <optional>

#include "common/common.h"
#include "core/renderer/d3d11graphics.h"

class GameWindow
{
private:
	WNDCLASSEX m_WindowClass = { 0 };
	LPCSTR m_ClassName;
	HINSTANCE m_AppInstance;
	HWND m_WindowHandler;
	RootexGraphics* m_GraphicsHandler = nullptr;

public:

	GameWindow(int xOffset, int yOffset, int width, int height, const std::string& title);
	GameWindow(const GameWindow&) = delete;
	GameWindow& operator=(const GameWindow&) = delete;  

	~GameWindow();

	int gameLoop();
	static LRESULT CALLBACK windowsMessageListener(HWND windowHandler, UINT msg, WPARAM wParam, LPARAM lParam);
	RootexGraphics* getGraphics();
	std::optional<int> GameWindow::ProcessMessages();
};
