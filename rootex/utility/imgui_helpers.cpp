#include "imgui_helpers.h"

#include "resource_loader.h"
#include "event_manager.h"

void RootexSelectableImage(const char* name, ImageResourceFile* image, Function<void(const String&)> onSelected)
{
	if (ImGui::Button((name + String("##Button")).c_str()))
	{
		igfd::ImGuiFileDialog::Instance()->OpenModal(name, name, SupportedFiles.at(ResourceFile::Type::Image), "game/assets/");
	}

	if (igfd::ImGuiFileDialog::Instance()->FileDialog(name))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk)
		{
			String filePathName = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName()).generic_string();
			onSelected(filePathName);
		}

		igfd::ImGuiFileDialog::Instance()->CloseDialog(name);
	}
	
	if (image && ImGui::ImageButton(image->getTexture()->getTextureResourceView(), { 50, 50 }))
	{
		EventManager::GetSingleton()->deferredCall(name, "EditorOpenFile", image->getPath().string());
	}
	if (image)
	{
		ImGui::SameLine();
		ImGui::Text(image->getPath().string().c_str());
	}
}

void RootexSelectableImageCube(const char* name, ImageCubeResourceFile* image, Function<void(const String&)> onSelected)
{
	if (ImGui::Button((name + String("##Button")).c_str()))
	{
		igfd::ImGuiFileDialog::Instance()->OpenModal(name, name, SupportedFiles.at(ResourceFile::Type::ImageCube), "game/assets/");
	}
	if (igfd::ImGuiFileDialog::Instance()->FileDialog(name))
	{
		if (igfd::ImGuiFileDialog::Instance()->IsOk)
		{
			String filePathName = OS::GetRootRelativePath(igfd::ImGuiFileDialog::Instance()->GetFilePathName()).generic_string();
			onSelected(filePathName);
		}

		igfd::ImGuiFileDialog::Instance()->CloseDialog(name);
	}

	if (image && ImGui::ImageButton(image->getTexture()->getTextureResourceView(), { 50, 50 }))
	{
		EventManager::GetSingleton()->deferredCall(name, "EditorOpenFile", image->getPath().string());
	}
	if (image)
	{
		ImGui::SameLine();
		ImGui::Text(image->getPath().string().c_str());
	}
}
