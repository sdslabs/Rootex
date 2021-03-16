#pragma once

#include "common/common.h"

#include "resource_loader.h"

class TextViewer
{
	Ref<TextResourceFile> m_TextResourceFile;

	void drawFileInfo();

public:
	Ref<ResourceFile> load(const FilePath& filePath);
	void unload();
	void draw(float deltaMilliseconds);
};
