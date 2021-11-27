#include "content_browser.h"

#include "editor/editor_system.h"

#include "utility/imgui_helpers.h"
#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

#include <filesystem>

ContentBrowser::ContentBrowser()
{
	// nothing for now
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
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
				}
			}
			ImGui::SameLine();
			ImGui::Text(std::filesystem::relative(m_CurrentDirectory, m_AssetsDirectory).string().c_str());
			ImGui::SameLine(ImGui::GetWindowWidth() - 85);
			if (ImGui::Button("Reload"))
			{
				m_ReloadPending = true;
			}
			EditorSystem::GetSingleton()->popFont();
			ImGui::Separator();

			for (auto const& directoryIterator : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				if (directoryIterator.is_directory())
				{
					if (ImGui::Button(directoryIterator.path().string().c_str()))
					{
						m_CurrentDirectory = directoryIterator.path();
					}
				}
				else
				{
					ImGui::Text(directoryIterator.path().string().c_str());
				}
			}
		}
		ImGui::End();
	}
}
