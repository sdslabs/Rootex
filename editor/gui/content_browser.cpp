#include "content_browser.h"

#include "editor/editor_system.h"

#include "utility/imgui_helpers.h"
#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void CALLBACK notifyFileSystemChanges(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	printf("Callback trigerred, yuayayayaya \n");
}

ContentBrowser::ContentBrowser()
{
	m_DirectoryImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\folder.png");
	m_ScriptImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\script.png");
	m_MusicImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\music.png");
	//OS::RegisterFileSystemWatcher(m_AssetsDirectory, &notifyFileSystemChanges);
}

ContentBrowser* ContentBrowser::GetSingleton()
{
	static ContentBrowser singleton;
	return &singleton;
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
					m_ReloadPending = true;
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

			if (m_ReloadPending)
			{
				try
				{
					Vector<FilePath> filepaths = OS::GetAllInDirectoryRoot(m_CurrentDirectory);
					m_thumbnail_cache.clear();
					for (FilePath directoryIterator : filepaths)
					{
						String directoryIteratorString = directoryIterator.string();
						if (OS::IsDirectory(directoryIteratorString))
						{
							m_thumbnail_cache[directoryIteratorString] = m_DirectoryImage;
						}
						else if (directoryIterator.extension().string() == ".wav")
						{
							m_thumbnail_cache[directoryIteratorString] = m_MusicImage;
						}
						else if (directoryIterator.extension().string() == ".png")
						{
							m_thumbnail_cache[directoryIteratorString] = ResourceLoader::CreateImageResourceFile(directoryIteratorString);
						}
						else
						{
							m_thumbnail_cache[directoryIteratorString] = m_ScriptImage;
						}
					}
					m_filepaths_cache = filepaths;
					m_ReloadPending = false;
				}
				catch (...)
				{
					WARN("Iteration on assets directory failed.")
				}
			}
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 1.f, 1.f, 0.5f));
			int id = 0;
			for (FilePath directoryIterator : m_filepaths_cache)
			{
				ImGui::PushID(id++);
				const String directoryIteratorString = directoryIterator.string();

				if (OS::IsDirectory(directoryIteratorString))
				{
					if (ImGui::ImageButton(m_thumbnail_cache[directoryIteratorString]->getTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_DirectoryImage->getTexture()->getHeight()) * m_IconWidth / ((float)m_DirectoryImage->getTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
					{
						m_CurrentDirectory = directoryIterator.string();
						m_ReloadPending = true;
					}
				}
				else if (directoryIterator.extension().string() == ".wav")
				{
					if (ImGui::ImageButton(m_thumbnail_cache[directoryIteratorString]->getTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_MusicImage->getTexture()->getHeight()) * m_IconWidth / ((float)m_MusicImage->getTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
					{
					}
				}
				else if (directoryIterator.extension().string() == ".json")
				{
					if (ImGui::ImageButton(m_thumbnail_cache[directoryIteratorString]->getTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_MusicImage->getTexture()->getHeight()) * m_IconWidth / ((float)m_MusicImage->getTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
					{
					}
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("SCENE_PAYLOAD", directoryIteratorString.c_str(), (directoryIteratorString.size() + 1) * sizeof(char), ImGuiCond_Once);
						ImGui::EndDragDropSource();
					}
				}
				else
				{
					if (ImGui::ImageButton(m_thumbnail_cache[directoryIteratorString]->getTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_ScriptImage->getTexture()->getHeight()) * m_IconWidth / ((float)m_ScriptImage->getTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
					{
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
