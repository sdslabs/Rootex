#pragma once

#include "theme.h"

class LightTheme : public Theme
{
public:
	void apply() const override
	{
		ImGui::StyleColorsLight();
		ImGuiStyle& style = ImGui::GetStyle();

		// Layout
		style.WindowPadding = ImVec2(15, 15);
		style.WindowRounding = 5.0f;
		style.FramePadding = ImVec2(5, 5);
		style.FrameRounding = 4.0f;
		style.ItemSpacing = ImVec2(12, 6);
		style.ItemInnerSpacing = ImVec2(8, 4);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 3.0f;

		// Colors
		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.86f, 0.86f, 0.90f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.95f, 0.95f, 0.95f, 0.50f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.95f, 0.95f, 0.96f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.75f, 0.75f, 0.80f, 0.60f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.65f, 0.65f, 0.70f, 0.80f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 0.50f, 0.75f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.60f, 0.60f, 0.70f, 0.50f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 0.50f, 0.75f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.75f, 0.75f, 0.85f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.85f, 0.95f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.65f, 0.65f, 0.90f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.80f, 0.80f, 0.90f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.85f, 0.85f, 0.95f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.65f, 0.85f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.50f, 0.80f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.60f, 0.70f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.80f, 0.80f, 0.85f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.92f, 0.92f, 0.90f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.75f, 0.75f, 0.85f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.30f, 0.60f, 0.90f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 0.50f, 0.75f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.30f, 0.45f, 0.60f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 0.50f, 0.75f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.85f, 0.85f, 0.90f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.70f, 0.70f, 0.75f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.85f, 0.85f, 0.90f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.95f, 0.95f, 0.95f, 0.30f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.60f, 0.90f, 0.50f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.60f, 1.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.30f, 0.60f, 0.90f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.90f, 0.90f, 0.90f, 0.60f);
	}

	String getName() const override
	{
		return "Light";
	}
};
