#include "material_viewer.h"

#include "core/resource_loader.h"
#include "core/renderer/material_library.h"

ResourceFile* MaterialViewer::load(const FilePath& filePath)
{
	m_OpenedMaterial = MaterialLibrary::GetMaterial(filePath.generic_string());
	return ResourceLoader::CreateTextResourceFile(filePath.generic_string());
}

void MaterialViewer::unload()
{
	m_OpenedMaterial.reset();
}

void MaterialViewer::draw(float deltaMilliseconds)
{
	m_OpenedMaterial->draw("Material Viewer");
}
