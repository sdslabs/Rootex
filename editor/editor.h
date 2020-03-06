#pragma once

#include "core/event_manager.h"
#include "script/interpreter.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

class VisualComponentGraph;

class Editor
{
	int m_DockSpaceID;
	bool m_FileSystemShow = true;

	unsigned int m_EditorStyleColorPushCount;
	unsigned int m_EditorStyleVarPushCount;

	struct Colors
	{
		ImVec4 m_Accent;
		ImVec4 m_MediumAccent;
		ImVec4 m_HeavyAccent;
		ImVec4 m_Inactive;
	} m_Colors;

	struct ToolbarSettings
	{
		bool m_IsActive = true;
	} m_ToolbarSettings;

	struct ViewportSettings
	{
		bool m_IsActive = true;
		bool m_IsClosed;

		ImVec2 m_ImageSize;
		ImVec4 m_ImageTint;
		ImVec4 m_ImageBorderColor;
		float m_AspectRatio;
	} m_ViewportSettings;

	struct HierarchySettings
	{
		bool m_IsActive = true;
	} m_HierarchySettings;

	struct InspectorSettings
	{
		bool m_IsActive = true;
	} m_InspectorSettings;

	struct OutputSettings
	{
		bool m_IsActive = true;
	} m_OutputSettings;

	struct FileSystemSettings
	{
		bool m_IsActive = true;
	} m_FileSystemSettings;

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
	void start(VisualComponentGraph* visualGraph);
	void end(VisualComponentGraph* visualGraph);
	Variant quit(const Event* event);
};
