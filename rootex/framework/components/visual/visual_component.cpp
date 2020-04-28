#include "visual_component.h"

VisualComponent::VisualComponent(const unsigned int& renderPassSetting, bool isVisible)
    : m_RenderPass((RenderPass)renderPassSetting)
    , m_IsVisible(isVisible)
    , m_TransformComponent(nullptr)
{
}

bool VisualComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			status = false;
		}
	}
	return status;
}

JSON::json VisualComponent::getJSON() const
{
	JSON::json j;

	j["renderPass"] = (int)m_RenderPass;
	j["isVisible"] = m_IsVisible;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void VisualComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	if (ImGui::BeginCombo("Render pass", m_RenderPassName.c_str()))
	{
		if (ImGui::Selectable("Main"))
		{
			m_RenderPass = RenderPassMain;
			m_RenderPassName = "Main";
		}
		if (ImGui::Selectable("Sky"))
		{
			m_RenderPass = RenderPassSky;
			m_RenderPassName = "Sky";
		}
		if (ImGui::Selectable("UI"))
		{
			m_RenderPass = RenderPassUI;
			m_RenderPassName = "UI";
		}
		if (ImGui::Selectable("Editor"))
		{
			m_RenderPass = RenderPassEditor;
			m_RenderPassName = "Editor";
		}
		ImGui::EndCombo();
	}
}
#endif // ROOTEX_EDITOR
