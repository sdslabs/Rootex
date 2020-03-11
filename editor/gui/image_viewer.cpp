#include "image_viewer.h"

#include "imgui.h"

void ImageViewer::draw()
{
	ImGui::Image(m_Texture->getTextureResourceView(), { (float)m_Texture->getWidth(), (float)m_Texture->getHeight() });
}

void ImageViewer::load(const FilePath& filePath)
{
	m_ImageResourceFile = ResourceLoader::CreateImageResourceFile(filePath.string());
	m_Texture.reset(new Texture(m_ImageResourceFile));
}

void ImageViewer::unload()
{
	m_Texture.reset();
}
