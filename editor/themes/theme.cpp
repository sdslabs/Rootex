#include "theme.h"

void setVec2(const JSON::json& j, const char* key, ImVec2& ref)
{
	if (j.contains(key))
	{
		if (j[key].is_array() && j[key].size() == 2)
		{
			ref = ImVec2(j[key][0], j[key][1]);
			return;
		}
		WARN("Theme key: " + key + " is not a valid array of size 2");
	}
}

void setFloat(const JSON::json& j, const char* key, float& ref)
{
	if (j.contains(key))
	{
		if (j[key].is_number())
		{
			ref = j[key].get<float>();
			return;
		}
		WARN("Theme key: " + key + " is not a valid number");
	}
}

void setColor(const JSON::json& j, const char* key, ImVec4& ref)
{
	if (j.contains(key))
	{
		if (j[key].is_array() && j[key].size() == 4)
		{
			ref = ImVec4(j[key][0], j[key][1], j[key][2], j[key][3]);
			return;
		}
		WARN("Theme key: " + key + " is not a valid array of size 4");
	}
}

void ThemeDefinition::apply() const
{
	ImGui::StyleColorsDark(); // Base reset

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	if (!m_ThemeData.contains("style") && !m_ThemeData.contains("colors"))
	{
		return;
	}

	if (m_ThemeData.contains("style"))
	{
		const auto& s = m_ThemeData["style"];
		setVec2(s, "WindowPadding", style.WindowPadding);
		setFloat(s, "WindowRounding", style.WindowRounding);
		setVec2(s, "FramePadding", style.FramePadding);
		setFloat(s, "FrameRounding", style.FrameRounding);
		setVec2(s, "ItemSpacing", style.ItemSpacing);
		setVec2(s, "ItemInnerSpacing", style.ItemInnerSpacing);
		setFloat(s, "IndentSpacing", style.IndentSpacing);
		setFloat(s, "ScrollbarSize", style.ScrollbarSize);
		setFloat(s, "ScrollbarRounding", style.ScrollbarRounding);
		setFloat(s, "GrabMinSize", style.GrabMinSize);
		setFloat(s, "GrabRounding", style.GrabRounding);
	}

	if (m_ThemeData.contains("colors"))
	{
		const auto& c = m_ThemeData["colors"];

		setColor(c, "Text", colors[ImGuiCol_Text]);
		setColor(c, "TextDisabled", colors[ImGuiCol_TextDisabled]);
		setColor(c, "WindowBg", colors[ImGuiCol_WindowBg]);
		setColor(c, "ChildBg", colors[ImGuiCol_ChildBg]);
		setColor(c, "PopupBg", colors[ImGuiCol_PopupBg]);
		setColor(c, "Border", colors[ImGuiCol_Border]);
		setColor(c, "BorderShadow", colors[ImGuiCol_BorderShadow]);
		setColor(c, "FrameBg", colors[ImGuiCol_FrameBg]);
		setColor(c, "FrameBgHovered", colors[ImGuiCol_FrameBgHovered]);
		setColor(c, "FrameBgActive", colors[ImGuiCol_FrameBgActive]);
		setColor(c, "TitleBg", colors[ImGuiCol_TitleBg]);
		setColor(c, "TitleBgActive", colors[ImGuiCol_TitleBgActive]);
		setColor(c, "TitleBgCollapsed", colors[ImGuiCol_TitleBgCollapsed]);
		setColor(c, "MenuBarBg", colors[ImGuiCol_MenuBarBg]);
		setColor(c, "ScrollbarBg", colors[ImGuiCol_ScrollbarBg]);
		setColor(c, "ScrollbarGrab", colors[ImGuiCol_ScrollbarGrab]);
		setColor(c, "ScrollbarGrabHovered", colors[ImGuiCol_ScrollbarGrabHovered]);
		setColor(c, "ScrollbarGrabActive", colors[ImGuiCol_ScrollbarGrabActive]);
		setColor(c, "CheckMark", colors[ImGuiCol_CheckMark]);
		setColor(c, "SliderGrab", colors[ImGuiCol_SliderGrab]);
		setColor(c, "SliderGrabActive", colors[ImGuiCol_SliderGrabActive]);
		setColor(c, "Button", colors[ImGuiCol_Button]);
		setColor(c, "ButtonHovered", colors[ImGuiCol_ButtonHovered]);
		setColor(c, "ButtonActive", colors[ImGuiCol_ButtonActive]);
		setColor(c, "Header", colors[ImGuiCol_Header]);
		setColor(c, "HeaderHovered", colors[ImGuiCol_HeaderHovered]);
		setColor(c, "HeaderActive", colors[ImGuiCol_HeaderActive]);
		setColor(c, "Separator", colors[ImGuiCol_Separator]);
		setColor(c, "SeparatorHovered", colors[ImGuiCol_SeparatorHovered]);
		setColor(c, "SeparatorActive", colors[ImGuiCol_SeparatorActive]);
		setColor(c, "ResizeGrip", colors[ImGuiCol_ResizeGrip]);
		setColor(c, "ResizeGripHovered", colors[ImGuiCol_ResizeGripHovered]);
		setColor(c, "ResizeGripActive", colors[ImGuiCol_ResizeGripActive]);
		setColor(c, "Tab", colors[ImGuiCol_Tab]);
		setColor(c, "TabHovered", colors[ImGuiCol_TabHovered]);
		setColor(c, "TabActive", colors[ImGuiCol_TabActive]);
		setColor(c, "TabUnfocused", colors[ImGuiCol_TabUnfocused]);
		setColor(c, "TabUnfocusedActive", colors[ImGuiCol_TabUnfocusedActive]);
		setColor(c, "DockingPreview", colors[ImGuiCol_DockingPreview]);
		setColor(c, "DockingEmptyBg", colors[ImGuiCol_DockingEmptyBg]);
		setColor(c, "PlotLines", colors[ImGuiCol_PlotLines]);
		setColor(c, "PlotLinesHovered", colors[ImGuiCol_PlotLinesHovered]);
		setColor(c, "PlotHistogram", colors[ImGuiCol_PlotHistogram]);
		setColor(c, "PlotHistogramHovered", colors[ImGuiCol_PlotHistogramHovered]);
		setColor(c, "TableHeaderBg", colors[ImGuiCol_TableHeaderBg]);
		setColor(c, "TableBorderStrong", colors[ImGuiCol_TableBorderStrong]);
		setColor(c, "TableBorderLight", colors[ImGuiCol_TableBorderLight]);
		setColor(c, "TableRowBg", colors[ImGuiCol_TableRowBg]);
		setColor(c, "TableRowBgAlt", colors[ImGuiCol_TableRowBgAlt]);
		setColor(c, "TextSelectedBg", colors[ImGuiCol_TextSelectedBg]);
		setColor(c, "DragDropTarget", colors[ImGuiCol_DragDropTarget]);
		setColor(c, "NavHighlight", colors[ImGuiCol_NavHighlight]);
		setColor(c, "NavWindowingHighlight", colors[ImGuiCol_NavWindowingHighlight]);
		setColor(c, "NavWindowingDimBg", colors[ImGuiCol_NavWindowingDimBg]);
		setColor(c, "ModalWindowDimBg", colors[ImGuiCol_ModalWindowDimBg]);
	}
}
