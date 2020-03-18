#pragma once

#include "imgui.h"
#include "resource_loader.h"

class ResourceSelector
{
public:
	static inline ResourceFile* DrawSelect(const Vector<ResourceFile::Type>& filters = {})
	{
		ResourceFile* selected = nullptr;
		if (ImGui::BeginCombo("##Select a File", ""))
		{
			for (auto&& filter : filters)
			{
				for (auto&& file : ResourceLoader::GetFilesOfType(filter))
				{
					if (ImGui::MenuItem(file->getPath().string().c_str(), ""))
					{
						selected = file;
					}
				}
			}
			ImGui::End();
		}
		return selected;
	}
};
