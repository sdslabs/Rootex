#include "image_viewer.h"

#include "editor/editor_system.h"

#include "imgui.h"

void ImageViewer::draw(float deltaMilliseconds)
{
	drawFileInfo();
	if (m_ImageResourceFile->isDirty())
	{
		ImGui::TextColored(EditorSystem::GetSingleton()->getColors().warning, "File may be changed on disk");
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			m_Texture->reload();
		}
	}
	ImGui::Separator();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
	ImGui::SliderFloat("##Zoom", &m_Zoom , m_MinZoom, m_MaxZoom, "Zoom %.3fx");
	ImGui::Image(m_Texture->getTextureResourceView(), { m_Zoom * (float)m_Texture->getWidth(), m_Zoom * (float)m_Texture->getHeight() }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, EditorSystem::GetSingleton()->getColors().accent);
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
