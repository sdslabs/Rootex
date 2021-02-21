#pragma once

#include "common/common.h"

#include "event_manager.h"
#include "resource_file.h"
#include "audio_player.h"
#include "image_viewer.h"
#include "text_viewer.h"
#include "material_viewer.h"

class FileViewer
{
	bool m_IsFileOpened;
	bool m_IsEventJustReceived;
	FilePath m_OpenFilePath;

	AudioPlayer m_AudioPlayer;
	ImageViewer m_ImageViewer;
	TextViewer m_TextViewer;
	MaterialViewer m_MaterialViewer;

	ResourceFile* m_OpenFile;

	void drawFileInfo();

	Variant openFile(const Event* event);

public:
	FileViewer();
	FileViewer(FileViewer&) = delete;
	~FileViewer() = default;

	void draw(float deltaMilliseconds);
};
