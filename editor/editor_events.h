#pragma once

#include "rootex/core/event.h"

struct EditorEvents
{
	/// Auto-saving event
	DEFINE_EVENT(EditorAutoSave);

	/// Close any scene opened in Inspector
	DEFINE_EVENT(EditorCloseScene);

	/// Create new scene from scene file path
	DEFINE_EVENT(EditorCreateNewScene, String);

	/// Create new material from [material path, material type]
	DEFINE_EVENT(EditorCreateNewMaterial);

	/// Open file at the path passed in inside File Viewer
	DEFINE_EVENT(EditorOpenFile, String);

	/// Open scene in Inspector
	DEFINE_EVENT(EditorOpenScene, Scene*);

	/// Set editor to ask to save before quitting
	DEFINE_EVENT(EditorSaveBeforeQuit);

	/// Set editor to save all
	DEFINE_EVENT(EditorSaveAll);

	/// Set editor to save all
	DEFINE_EVENT(EditorExportScene);
};
