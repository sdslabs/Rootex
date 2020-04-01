#include "visual_component.h"

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
