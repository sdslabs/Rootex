#include "content_browser.h"

#include "editor/editor_system.h"
#include "core/resource_loader.h"

#include "utility/imgui_helpers.h"
#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void CALLBACK notifyFileChanges(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	bool* ReloadPending = (bool*)lpParam;
	OS::RegisterFileChangesWatcher("game\\assets\\", &notifyFileChanges, ReloadPending);
	*ReloadPending = true;
}

void CALLBACK notifyDirectoryChanges(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	bool* ReloadPending = (bool*)lpParam;
	OS::RegisterDirectoryChangesWatcher("game\\assets\\", &notifyDirectoryChanges, ReloadPending);
	*ReloadPending = true;
}

ContentBrowser::ContentBrowser()
{
	m_ReloadPending = true;
	m_DirectoryImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\folder.png");
	m_ScriptImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\script.png");
	m_MusicImage = ResourceLoader::CreateImageResourceFile("editor\\assets\\icons\\music.png");
	OS::RegisterDirectoryChangesWatcher(m_AssetsDirectory, &notifyDirectoryChanges, &m_ReloadPending);
	OS::RegisterFileChangesWatcher(m_AssetsDirectory, &notifyFileChanges, &m_ReloadPending);
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
					Vector<FilePath> temp_files;
					Vector<FilePath> temp_dirs;
					m_thumbnail_cache.clear();
					for (FilePath directoryIterator : filepaths)
					{
						String directoryIteratorString = directoryIterator.string();
						if (OS::IsDirectory(directoryIteratorString))
						{
							m_thumbnail_cache[directoryIteratorString] = m_DirectoryImage;
							temp_dirs.push_back(directoryIteratorString);
						}
						else if (directoryIterator.extension().string() == ".wav")
						{
							m_thumbnail_cache[directoryIteratorString] = m_MusicImage;
							temp_files.push_back(directoryIteratorString);
						}
						else if (directoryIterator.extension().string() == ".png")
						{
							m_thumbnail_cache[directoryIteratorString] = ResourceLoader::CreateImageResourceFile(directoryIteratorString);
							temp_files.push_back(directoryIteratorString);
						}
						else
						{
							m_thumbnail_cache[directoryIteratorString] = m_ScriptImage;
							temp_files.push_back(directoryIteratorString);
						}
					}
					m_filepaths_cache = temp_files;
					m_dir_cache = temp_dirs;
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
			for (FilePath directoryIterator : m_dir_cache)
			{
				ImGui::PushID(id++);
				const String directoryIteratorString = directoryIterator.string();
				if (ImGui::ImageButton(m_DirectoryImage->getGPUTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_DirectoryImage->getGPUTexture()->getHeight()) * m_IconWidth / ((float)m_DirectoryImage->getGPUTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
				{
					m_CurrentDirectory = directoryIterator.string();
					m_ReloadPending = true;
				}

				ImGui::PopID();
				ImGui::Text(directoryIterator.filename().string().c_str());
				ImGui::NextColumn();
			}
			for (FilePath directoryIterator : m_filepaths_cache)
			{
				ImGui::PushID(id++);
				const String directoryIteratorString = directoryIterator.string();

				if (ImGui::ImageButton(m_thumbnail_cache[directoryIteratorString]->getGPUTexture()->getTextureResourceView(), { m_IconWidth, ((float)m_MusicImage->getGPUTexture()->getHeight()) * m_IconWidth / ((float)m_MusicImage->getGPUTexture()->getWidth()) }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, 12, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }))
				{
					if (m_PayloadTypes.find(directoryIterator.extension().string()) != m_PayloadTypes.end())
					{
						String payload_type = m_PayloadTypes.at(directoryIterator.extension().string())[0];
						if (directoryIterator.extension().string() == ".lua" || directoryIterator.extension().string() == ".rml")
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { directoryIteratorString, (int)ResourceFile::Type::Text });
						}
						else if (payload_type == "MODEL_PAYLOAD")
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { directoryIteratorString, (int)ResourceFile::Type::Model });
						}
						else if (payload_type == "IMAGE_PAYLOAD")
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { directoryIteratorString, (int)ResourceFile::Type::Image });
						}
						else if (payload_type == "AUDIO_PAYLOAD")
						{
							EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { directoryIteratorString, (int)ResourceFile::Type::Audio });
						}
					}
				}
				if (m_PayloadTypes.find(directoryIterator.extension().string()) != m_PayloadTypes.end())
				{
					for (const char* it : m_PayloadTypes.at(directoryIterator.extension().string()))
					{
						if (ImGui::BeginDragDropSource())
						{
							ImGui::SetDragDropPayload(it, directoryIteratorString.c_str(), (directoryIteratorString.size() + 1) * sizeof(char), ImGuiCond_Once);
							ImGui::EndDragDropSource();
						}
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
