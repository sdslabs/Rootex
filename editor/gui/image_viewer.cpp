#include "image_viewer.h"

#include "imgui.h"

void ImageViewer::draw()
{
	ImGui::Image(m_ImageResourceFile->getData()->getRawData()->data(), ImGui::GetContentRegionAvail());
}

void ImageViewer::load(const FilePath& filePath)
{
	m_ImageResourceFile = ResourceLoader::CreateImageResourceFile(filePath.string());
}

void ImageViewer::unload()
{
}
