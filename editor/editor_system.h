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
#include "ImGuiFileDialogConfig.h"

#include "Tracy/Tracy.hpp"

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
	ImFont* m_EditorFontItalic;
	ImFont* m_EditorFontBold;
	
	struct Colors
	{
		ImVec4 accent;
		ImVec4 failAccent;
		ImVec4 mediumAccent;
		ImVec4 heavyAccent;
		ImVec4 background;
		ImVec4 inactive;
		ImVec4 success;
		ImVec4 failure;
		ImVec4 warning;
		ImVec4 white;
		ImVec4 itemBackground;
		ImVec4 text;
	} m_Colors;

	struct Icons
	{
		const char* lua = ICON_ROOTEX_FILE_CODE_O;
		const char* font = ICON_ROOTEX_FONT;
		const char* text = ICON_ROOTEX_FILE_TEXT;
		const char* audio = ICON_ROOTEX_FILE_AUDIO_O;
		const char* model = ICON_ROOTEX_FORT_AWESOME;
		const char* image = ICON_ROOTEX_FILE_IMAGE_O;
	} m_Icons;

	Ptr<FileSystemDock> m_FileSystem;
	Ptr<HierarchyDock> m_Hierarchy;
	Ptr<OutputDock> m_Output;
	Ptr<ToolbarDock> m_Toolbar;
	Ptr<ViewportDock> m_Viewport;
	Ptr<InspectorDock> m_Inspector;
	Ptr<FileViewer> m_FileViewer;

	EditorSystem();
	EditorSystem(const EditorSystem&) = delete;
	~EditorSystem();

	void drawDefaultUI(float deltaMilliseconds);

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
	void pushItalicFont();
	void popFont();

	const Colors& getColors() const { return m_Colors; }

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
