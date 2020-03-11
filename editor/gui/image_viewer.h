#pragma once

#include "common/common.h"

#include "resource_loader.h"

class ImageViewer
{
	ImageResourceFile* m_ImageResourceFile;

public:
	void load(const FilePath& filePath);
	void unload();
	void draw();
};
