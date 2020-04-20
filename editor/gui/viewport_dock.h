#pragma once

#include "script/interpreter.h"

#include "vendor/ImGUI/imgui.h"
#include "vendor/ImGUI/imgui_impl_dx11.h"
#include "vendor/ImGUI/imgui_impl_win32.h"

class ViewportDock
{
public:
	struct ViewportDockSettings
	{
		bool m_IsActive = true;
		bool m_IsClosed;

		ImVec2 m_ImageSize;
		ImVec4 m_ImageTint;
		ImVec4 m_ImageBorderColor;
		float m_AspectRatio;
	};

private:
	ViewportDockSettings m_ViewportDockSettings;
	Matrix m_ApplyCameraMatrix = Matrix::Identity;
	Ref<Entity> m_EditorCamera;
	bool m_IsCameraMoving;
	float m_EditorCameraSpeed = 0.1f;

public:
	ViewportDock(const JSON::json& viewportJSON);
	ViewportDock(ViewportDock&) = delete;
	~ViewportDock() = default;

	void draw();
	
	ViewportDockSettings& getSettings() { return m_ViewportDockSettings; }
	void setActive(bool enabled) { m_ViewportDockSettings.m_IsActive = enabled; }
};
