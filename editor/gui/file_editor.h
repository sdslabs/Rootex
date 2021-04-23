#pragma once

#include "common/common.h"
#include "event_manager.h"

#include "ImGuiColorTextEdit/TextEditor.h"

class FileEditor
{
	EventBinder<FileEditor> m_Binder;

	TextEditor m_Editor;

	String m_EditFilePath;

	Variant editFile(const Event* e);

public:
	FileEditor();

	void draw(float deltaMilliseconds);
};
