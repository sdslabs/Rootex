#pragma once

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

#include "script/interpreter.h"

class VisualComponentGraph;

class Editor
{
	int m_DockSpaceID;
	bool m_FileSystemShow = true;
	LuaInterpreter m_EditorConfig;

	unsigned int m_EditorStyleColorPushCount;
	unsigned int m_EditorStyleVarPushCount;

	struct Colors
	{
		ImVec4 m_Accent;
		ImVec4 m_MediumAccent;
		ImVec4 m_HeavyAccent;
	} m_Colors;

	struct ViewportSettings
	{
		ImVec2 m_ImageSize;
		ImVec4 m_ImageTint;
		ImVec4 m_ImageBorderColor;
		float m_AspectRatio;

		bool m_IsClosed;
	} m_ViewportSettings;

	Vector<String> m_Logs;

	Editor() = default;
	Editor(Editor&) = delete;
	~Editor();

	void applyDefaultUI();
	void applyMainMenu(const ImGuiWindowFlags& windowFlags, bool optFullscreen, const ImGuiDockNodeFlags& dockspaceFlags);
	
	void applyDocks();
	void applyFileSystemDock();
	void applyToolbarDock();
	void applyHierarchyDock();
	void applyInspectorDock();
	void applyOutputDock();
	void applyViewportDock();

	void pushEditorStyleColors();
	void pushEditorStyleVars();
	
	void logToOutput(const String& log);

	friend class OS;

public:
	static Editor* GetSingleton();


	void initialize(HWND hWnd);
	void draw(VisualComponentGraph* visualGraph);
	void end(VisualComponentGraph* visualGraph);
};
