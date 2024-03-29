#include "image_viewer.h"

#include "editor/editor_system.h"
#include "core/resource_files/image_resource_file.h"

#include "imgui.h"

void ImageViewer::draw(float deltaMilliseconds)
{
	drawFileInfo();
	if (m_ImageResourceFile->isDirty())
	{
		ImGui::TextColored(EditorSystem::GetSingleton()->getWarningColor(), "File may be changed on disk");
		ImGui::SameLine();
		if (ImGui::Button("Reload"))
		{
			m_ImageResourceFile->reimport();
		}
	}
	ImGui::Separator();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
	ImGui::SliderFloat("##Zoom", &m_Zoom, m_MinZoom, m_MaxZoom, "Zoom %.3fx");
	ImGui::Image(m_ImageResourceFile->getGPUTexture()->getTextureResourceView(), { m_Zoom * (float)m_ImageResourceFile->getGPUTexture()->getWidth(), m_Zoom * (float)m_ImageResourceFile->getGPUTexture()->getHeight() }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void ImageViewer::drawFileInfo()
{
	ImGui::Columns(2);

	ImGui::Text("Dimensions");
	ImGui::NextColumn();
	ImGui::Text((std::to_string(m_ImageResourceFile->getGPUTexture()->getWidth()) + "x" + std::to_string(m_ImageResourceFile->getGPUTexture()->getHeight())).c_str());
	ImGui::NextColumn();

	ImGui::Text("Mip Levels");
	ImGui::NextColumn();
	ImGui::Text(std::to_string(m_ImageResourceFile->getGPUTexture()->getMipLevels()).c_str());
	ImGui::NextColumn();

	ImGui::Columns(1);
}

Ref<ResourceFile> ImageViewer::load(const FilePath& filePath)
{
	m_ImageResourceFile = ResourceLoader::CreateImageResourceFile(filePath.string());
	return m_ImageResourceFile;
}

void ImageViewer::unload()
{
	m_ImageResourceFile = nullptr;
}
