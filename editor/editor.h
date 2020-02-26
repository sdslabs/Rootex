#pragma once

#include <Windows.h>
 
#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

class VisualComponentGraph;

class Editor
{
	int m_DockSpaceID;
	float m_ViewportWidth;
	float m_ViewportHeight;
	bool m_ViewportZoomEnabled;
	float m_ViewportZoom = 1.0f;

	bool m_FileSystemShow = true;

	Editor() = default;
	Editor(Editor&) = delete;
	~Editor();

	void applyDockspace();
	void applyDocks();

	void applyViewportDock();

	void applyToolbarDock();

public:
	static Editor* GetSingleton();

	void initialize(HWND hWnd);
	void begin(VisualComponentGraph* visualGraph);
	void end(VisualComponentGraph* visualGraph);
};
