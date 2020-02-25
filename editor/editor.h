#pragma once

#include <Windows.h>
 
#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

class VisualComponentGraph;

class Editor
{
	int m_DockSpaceID;

	Editor() = default;
	Editor(Editor&) = delete;
	~Editor();

	void applyDockspace();

public:
	static Editor* GetSingleton();

	void initialize(HWND hWnd);
	void begin(VisualComponentGraph* visualGraph);
	void end(VisualComponentGraph* visualGraph);
};
