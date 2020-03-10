#include "file_system_dock.h"

#include "editor/editor.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void FileSystemDock::drawFileSystemTree(const FilePath& path)
{
	if (ImGui::TreeNodeEx(path.string().c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Indent();
		for (auto&& item : OS::GetDirectoriesInDirectory(path.string()))
		{
			drawFileSystemTree(item);
		}

		for (auto&& item : OS::GetFilesInDirectory(path.string()))
		{
			if (ImGui::Selectable(item.filename().string().c_str(), m_OpenedFileName == item.string()))
			{
				m_OpenedFileName = item.string();
			}
		}
		ImGui::Unindent();
	}
}

void FileSystemDock::draw()
{
	if (m_FileSystemSettings.m_IsActive)
	{
		if (ImGui::Begin("File System"))
		{
			drawFileSystemTree("./");
		}
		ImGui::End();
	}
}
