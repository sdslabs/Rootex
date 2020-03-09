#include "file_system_dock.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

void FileSystemDock::draw()
{
	if (m_FileSystemSettings.m_IsActive)
	{
		if (ImGui::Begin("File System"))
		{
		}
		ImGui::End();
	}
}
