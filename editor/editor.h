#pragma once

#include "core/event_manager.h"
#include "script/interpreter.h"
#include "systems/serialization_system.h"

#include "gui/file_system_dock.h"
#include "gui/hierarchy_dock.h"
#include "gui/output_dock.h"
#include "gui/toolbar_dock.h"
#include "gui/viewport_dock.h"
#include "gui/inspector_dock.h"
#include "gui/file_viewer.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

class HierarchyGraph;

class Editor
{
	int m_DockSpaceID;

	unsigned int m_EditorStyleColorPushCount;
	unsigned int m_EditorStyleVarPushCount;

	String m_CurrentLevelName;

	struct Colors
	{
		ImVec4 m_Accent;
		ImVec4 m_FailAccent;
		ImVec4 m_MediumAccent;
		ImVec4 m_HeavyAccent;
		ImVec4 m_Inactive;
		ImVec4 m_Success;
		ImVec4 m_Failure;
		ImVec4 m_Warning;
		ImVec4 m_White;
	} m_Colors;

	Ptr<FileSystemDock> m_FileSystem;
	Ptr<HierarchyDock> m_Hierarchy;
	Ptr<OutputDock> m_Output;
	Ptr<ToolbarDock> m_Toolbar;
	Ptr<ViewportDock> m_Viewport;
	Ptr<InspectorDock> m_Inspector;
	Ptr<FileViewer> m_FileViewer;

	SerializationSystem m_SerializationSystem;

	Editor() = default;
	Editor(Editor&) = delete;
	~Editor();

	void drawDefaultUI();

	void pushEditorStyleColors();
	void pushEditorStyleVars();

	Variant saveAll(const Event* event);
	Variant autoSave(const Event* event);
	Variant openLevel(const Event* event);
	Variant createNewLevel(const Event* event);
	Variant createNewEntity(const Event* event);

public:
	static Editor* GetSingleton();

	void initialize(HWND hWnd, const JSON::json& projectJSON);
	void render();
	void quit();

	const Colors& getColors() const { return m_Colors; }
	const String& getCurrentLevelName() const { return m_CurrentLevelName; }
};
