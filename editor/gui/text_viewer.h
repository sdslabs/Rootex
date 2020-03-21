#pragma once

#include "common/common.h"

#include "resource_loader.h"

class TextViewer
{
	TextResourceFile* m_TextResourceFile;

	void drawFileInfo();

public:
	ResourceFile* load(const FilePath& filePath);
	void unload();
	void draw();
};
