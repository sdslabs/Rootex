#include "file_system_dock.h"

#include "editor/editor.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void FileSystemDock::drawFileSystemTree(const FilePath& path)
{
	FilePath dirPath = path;
	if (ImGui::TreeNodeEx(path.lexically_relative(dirPath.remove_filename()).string().c_str(), ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed))
	{
		for (auto&& item : OS::GetDirectoriesInDirectory(path.string()))
		{
			drawFileSystemTree(item);
		}

		for (auto&& item : OS::GetFilesInDirectory(path.string()))
		{
			if (ImGui::Selectable(item.filename().string().c_str(), m_OpenedFileName == item.string()))
			{
				m_OpenedFileName = item.string();
				EventManager::GetSingleton()->call("OpenFile", "EditorOpenFile", item);
			}
		
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Resource Drop", item.string().c_str(), item.string().size() + 1);
				ImGui::Text(item.string().c_str());
				ImGui::EndDragDropSource();
			}
		}
		ImGui::TreePop();
	}
}

void FileSystemDock::draw()
{
	if (m_FileSystemSettings.m_IsActive)
	{
		if (ImGui::Begin("File System"))
		{
			drawFileSystemTree("game/assets/");
		}
		ImGui::End();
	}
}
