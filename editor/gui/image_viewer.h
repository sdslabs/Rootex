#pragma once

#include "common/common.h"

#include "rootex/core/renderer/texture.h"
#include "resource_loader.h"

class ImageViewer
{
	Ref<Texture> m_Texture;
	ImageResourceFile* m_ImageResourceFile;

public:
	void load(const FilePath& filePath);
	void unload();
	void draw();
};
