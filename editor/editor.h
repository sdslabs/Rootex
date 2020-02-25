#pragma once

#include <Windows.h>
 
#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

class VisualComponentGraph;

class Editor
{
	Editor() = default;
	Editor(Editor&) = delete;
	~Editor();

public:
	static Editor* GetSingleton();

	void initialize(HWND hWnd);
	void begin(VisualComponentGraph* visualGraph);
	void displayVisualGraph(VisualComponentGraph* graph);
	void end(VisualComponentGraph* visualGraph);
};
