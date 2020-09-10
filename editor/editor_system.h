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
#include "gui/classes_dock.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

class HierarchyGraph;

class EditorSystem : public System
{
	int m_DockSpaceID;
	String m_MenuAction;
	String m_PopupCause;
	unsigned int m_EditorStyleColorPushCount;
	unsigned int m_EditorStyleVarPushCount;
	bool m_WireframeMode = false;
	bool m_CollisionMode = false;
	bool m_WorldMode = true;

	ImFont* m_EditorFont;
	ImFont* m_EditorFontBold;

	struct Colors
	{
		ImVec4 m_Accent;
		ImVec4 m_FailAccent;
		ImVec4 m_MediumAccent;
		ImVec4 m_HeavyAccent;
		ImVec4 m_Background;
		ImVec4 m_Inactive;
		ImVec4 m_Success;
		ImVec4 m_Failure;
		ImVec4 m_Warning;
		ImVec4 m_White;
		ImVec4 m_ItemBackground;
		ImVec4 m_Text;
	} m_Colors;

	Ptr<FileSystemDock> m_FileSystem;
	Ptr<HierarchyDock> m_Hierarchy;
	Ptr<OutputDock> m_Output;
	Ptr<ToolbarDock> m_Toolbar;
	Ptr<ViewportDock> m_Viewport;
	Ptr<InspectorDock> m_Inspector;
	Ptr<FileViewer> m_FileViewer;
	Ptr<ClassesDock> m_Classes;

	EditorSystem();
	EditorSystem(const EditorSystem&) = delete;
	~EditorSystem();

	void drawDefaultUI();

	void pushEditorStyleColors();
	void pushEditorStyleVars();

	void showDocumentation(const String& name, const sol::table& table);

	Variant saveAll(const Event* event);
	Variant autoSave(const Event* event);
	Variant saveBeforeQuit(const Event* event);
	Variant createNewLevel(const Event* event);
	Variant createNewEntity(const Event* event);
	Variant createNewMaterial(const Event* event);

public:
	static EditorSystem* GetSingleton();

	bool initialize(const JSON::json& systemData) override;
	void update(float deltaMilliseconds) override;

	void pushRegularFont();
	void pushBoldFont();
	void popFont();

	const Colors& getColors() const { return m_Colors; }

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
