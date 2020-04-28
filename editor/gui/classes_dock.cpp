#include "classes_dock.h"

#include "rootex/os/os.h"

#include "imgui.h"

void ClassesDock::showClasses(const String& directory)
{
	for (auto& file : OS::GetAllInDirectory("game/assets/classes"))
	{
		if (OS::IsFile(file.string()))
		{
			ImGui::Selectable(file.filename().generic_string().c_str());

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("EntityClass", file.generic_string().c_str(), file.generic_string().size() + 1);
				ImGui::Text(file.generic_string().c_str());
				ImGui::EndDragDropSource();
			}
		}
		else
		{
			showClasses(file.string());
		}
	}
}

void ClassesDock::draw()
{
	ImGui::Begin("Classes");
	showClasses("game/assets/classes");
	ImGui::End();
}
