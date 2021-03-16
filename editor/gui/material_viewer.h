#pragma once

#include "common/common.h"

#include "resource_file.h"
#include "rootex/core/renderer/material.h"

class MaterialViewer
{
	Ref<Material> m_OpenedMaterial;

public:
	Ref<ResourceFile> load(const FilePath& filePath);
	void unload();

	void draw(float deltaMilliseconds);
};
