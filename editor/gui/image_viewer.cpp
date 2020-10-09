#include "image_viewer.h"

#include "editor/editor_system.h"
#include "core/resource_files/image_resource_file.h"

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
			m_ImageResourceFile->reimport();
		}
	}
	ImGui::Separator();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
	ImGui::SliderFloat("##Zoom", &m_Zoom , m_MinZoom, m_MaxZoom, "Zoom %.3fx");
	ImGui::Image(m_ImageResourceFile->getTexture()->getTextureResourceView(), { m_Zoom * (float)m_ImageResourceFile->getTexture()->getWidth(), m_Zoom * (float)m_ImageResourceFile->getTexture()->getHeight() }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, EditorSystem::GetSingleton()->getColors().accent);
}

void ImageViewer::drawFileInfo()
{
	ImGui::Columns(2);

	ImGui::Text("Dimensions");
	ImGui::NextColumn();
	ImGui::Text((std::to_string(m_ImageResourceFile->getTexture()->getWidth()) + "x" + std::to_string(m_ImageResourceFile->getTexture()->getHeight())).c_str());
	ImGui::NextColumn();

	ImGui::Text("Mip Levels");
	ImGui::NextColumn();
	ImGui::Text(std::to_string(m_ImageResourceFile->getTexture()->getMipLevels()).c_str());
	ImGui::NextColumn();

	ImGui::Columns(1);
}

ResourceFile* ImageViewer::load(const FilePath& filePath)
{
	m_ImageResourceFile = ResourceLoader::CreateImageResourceFile(filePath.string());
	return m_ImageResourceFile;
}

void ImageViewer::unload()
{
	m_ImageResourceFile = nullptr;
}
