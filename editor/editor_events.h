#pragma once

#include "rootex/core/event.h"

struct EditorEvents
{
	/// Auto-saving event
	DEFINE_EVENT(EditorAutoSave);

	/// Close any scene opened in Inspector
	DEFINE_EVENT(EditorSceneIsClosing);

	/// Create new scene from scene file path
	DEFINE_EVENT(EditorCreateNewScene, String);

	/// Create new file at path
	DEFINE_EVENT(EditorCreateNewFile, String);

	/// Open file at the path passed in inside File Viewer with the ResourceFile::Type to load it as
	DEFINE_EVENT(EditorOpenFile, String, int);

	/// Open scene in Inspector
	DEFINE_EVENT(EditorOpenScene, Scene*);

	/// Set editor to ask to save before quitting
	DEFINE_EVENT(EditorSaveBeforeQuit);

	/// Set editor to save all
	DEFINE_EVENT(EditorSaveAll);

	/// Reset editor window states
	DEFINE_EVENT(EditorReset);
};
