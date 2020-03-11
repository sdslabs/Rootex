#pragma once

#include "common/common.h"

#include "rootex/core/renderer/texture.h"
#include "resource_loader.h"

class ImageViewer
{
	Ref<Texture> m_Texture;
	ImageResourceFile* m_ImageResourceFile;
	const float m_ZoomSliderWidth = 40.0f;
	const float m_ZoomSliderHeight = 500.0f;
	const float m_MaxZoom = 3.0f;
	const float m_MinZoom = 0.0f;
	float m_Zoom = 1.0f;

	void drawFileInfo();

public:
	ResourceFile* load(const FilePath& filePath);
	void unload();
	void draw();
};
