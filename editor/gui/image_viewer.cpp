#include "image_viewer.h"

#include "editor/editor.h"

#include "imgui.h"

void ImageViewer::draw()
{
	drawFileInfo();
	ImGui::Separator();
	ImGui::Image(m_Texture->getTextureResourceView(), { (float)m_Texture->getWidth(), (float)m_Texture->getHeight() });
	if (m_ImageResourceFile->isDirty())
	{
		ImGui::TextColored(Editor::GetSingleton()->getColors().m_Warning, "File may be changed on disk");
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			m_Texture->reload();
		}
	}
}

void ImageViewer::drawFileInfo()
{
	ImGui::Columns(2);

	ImGui::Text("Dimensions");
	ImGui::NextColumn();
	ImGui::Text((std::to_string(m_Texture->getWidth()) + "x" + std::to_string(m_Texture->getHeight())).c_str());
	ImGui::NextColumn();

	ImGui::Text("Mip Levels");
	ImGui::NextColumn();
	ImGui::Text(std::to_string(m_Texture->getMipLevels()).c_str());
	ImGui::NextColumn();

	ImGui::Columns(1);
}

ResourceFile* ImageViewer::load(const FilePath& filePath)
{
	m_ImageResourceFile = ResourceLoader::CreateImageResourceFile(filePath.string());
	m_Texture.reset(new Texture(m_ImageResourceFile));

	return m_ImageResourceFile;
}

void ImageViewer::unload()
{
	m_Texture.reset();
}
