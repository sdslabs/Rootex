#include "classes_dock.h"

#include "rootex/os/os.h"

#include "imgui.h"

void ClassesDock::showClasses(const String& directory)
{
	for (auto& file : OS::GetAllInDirectory("game/assets/classes"))
	{
		if (OS::IsDirectory(file.string()))
		{
			ImGui::Selectable(file.filename().generic_string().c_str());

			if (ImGui::BeginDragDropSource())
			{
				String filePath = file.string() + "/" + file.filename().generic_string() + ".entity.json";
				ImGui::SetDragDropPayload("EntityClass", filePath.c_str(), filePath.size() + 1);
				ImGui::Text(file.generic_string().c_str());
				ImGui::EndDragDropSource();
			}
		}
	}
}

void ClassesDock::draw(float deltaMilliseconds)
{
	ImGui::Begin("Classes");
	showClasses("game/assets/classes");
	ImGui::End();
}
