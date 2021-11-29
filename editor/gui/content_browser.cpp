#include "content_browser.h"

#include "editor/editor_system.h"

#include "utility/imgui_helpers.h"
#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

ContentBrowser::ContentBrowser()
{
	m_DirectoryImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\folder.png");
	m_ScriptImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\script.png");
	m_MusicImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\music.png");
}

void ContentBrowser::draw(float deltaMilliseconds)
{
	ZoneScoped;
	if (m_ContentBrowserSettings.m_IsActive)
	{
		if (ImGui::Begin("Content Browser"))
		{
			EditorSystem::GetSingleton()->pushBoldFont();
			if (m_CurrentDirectory != m_AssetsDirectory)
			{
				if (ImGui::Button("<-"))
				{
					m_CurrentDirectory = OS::GetParentPath(m_CurrentDirectory).string();
				}
			}
			ImGui::SameLine();
			ImGui::Text(OS::GetRelativePath(m_CurrentDirectory, m_AssetsDirectory).string().c_str());
			ImGui::SameLine(ImGui::GetWindowWidth() - 85);
			if (ImGui::Button("Reload"))
			{
				m_ReloadPending = true;
			}
			EditorSystem::GetSingleton()->popFont();
			ImGui::Separator();

			int num_columns = ImGui::GetContentRegionAvailWidth() / (m_IconWidth + 30);
			if (num_columns < 1)
			{
				num_columns = 1;
			}
			ImGui::Columns(num_columns, 0, false);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.5f));
			int id = 0;
			for (FilePath directoryIterator : OS::GetAllInDirectory(m_CurrentDirectory))
			{
				ImGui::PushID(id++);
				String directoryIteratorString = directoryIterator.string();
				if (OS::IsDirectory(directoryIteratorString))
				{
					if (ImGui::ImageButton(m_DirectoryImage->getTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_DirectoryImage->getTexture()->getHeight()) * m_IconWidth / ((float)m_DirectoryImage->getTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
					{
						m_CurrentDirectory = directoryIterator.string();
						//std::cout << directoryIterator.path().string().c_str() << std::endl;
						//std::cout << m_CurrentDirectory.string().c_str() << std::endl;
					}
				}
				else if (directoryIterator.extension().string() == ".wav")
				{
					if (ImGui::ImageButton(m_MusicImage->getTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_MusicImage->getTexture()->getHeight()) * m_IconWidth / ((float)m_MusicImage->getTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
					{
						//std::cout << directoryIterator.string().c_str() << std::endl;
					}
				}
				else
				{
					if (ImGui::ImageButton(m_ScriptImage->getTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_ScriptImage->getTexture()->getHeight()) * m_IconWidth / ((float)m_ScriptImage->getTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
					{
						//std::cout << directoryIterator.string().c_str() << std::endl;
					}
				}
				ImGui::PopID();
				ImGui::Text(directoryIterator.filename().string().c_str());
				ImGui::NextColumn();
			}

			ImGui::PopStyleColor(3);

			ImGui::Columns(5);
		}
		ImGui::End();
	}
}
