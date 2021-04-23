#pragma once

#include "common/common.h"

#include "resource_file.h"
#include "core/resource_files/material_resource_file.h"

class MaterialViewer
{
	Ref<MaterialResourceFile> m_OpenedMaterial;

public:
	Ref<ResourceFile> load(const FilePath& filePath);
	void unload();

	void draw(float deltaMilliseconds);
};
