#include "inspector_dock.h"

#include "entity.h"
#include "framework/component.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

Variant InspectorDock::openEntity(const Event* event)
{
	Ref<Entity> entity = Extract(Ref<Entity>, event->getData());
	m_OpenedEntity = entity.get();

	return true;
}

InspectorDock::InspectorDock()
    : m_OpenedEntity(nullptr)
{
	BIND_EVENT_MEMBER_FUNCTION("EditorInspectorOpenEntity", openEntity);
}

static int TextInputResizeCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		String* myStr = (String*)data->UserData;
		myStr->resize(data->BufSize);
		data->Buf = myStr->data();
	}
	return 0;
}

void InspectorDock::draw()
{
	if (ImGui::Begin("Inspector"))
	{
		if (m_OpenedEntity)
		{
			ImGui::LabelText("", "%s", m_OpenedEntity->getName().c_str());
			for (auto& component : m_OpenedEntity->getAllComponents())
			{
				if (ImGui::TreeNodeEx(component.second->getName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
				{
					component.second->draw();
				}
			}
		}
	}
	ImGui::End();
}
