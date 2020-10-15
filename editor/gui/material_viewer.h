#pragma once

#include "common/common.h"

#include "rootex/core/renderer/material.h"

class MaterialViewer
{
	Ref<Material> m_OpenedMaterial;
	
public:
	ResourceFile* load(const FilePath& filePath);
	void unload();

	void draw(float deltaMilliseconds);
};
