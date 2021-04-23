#pragma once

#include "script/interpreter.h"
#include "scene.h"

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
		float m_AspectRatio;
	};

private:
	Scene* m_EditorCamera;
	Scene* m_EditorGrid;
	ViewportDockSettings m_ViewportDockSettings;
	Matrix m_ApplyCameraMatrix = Matrix::Identity;

	bool m_IsCameraMoving = false;

	float m_EditorCameraPitch = 0.0f;
	float m_EditorCameraYaw = 0.0f;
	float m_EditorCameraSpeed = 10.0f;
	float m_EditorCameraSensitivity = 300.0f;
	float m_EditorCameraRotationNormalizer = 1000.0f;

public:
	ViewportDock(const JSON::json& viewportJSON);
	ViewportDock(ViewportDock&) = delete;
	~ViewportDock() = default;

	void draw(float deltaMilliseconds);

	ViewportDockSettings& getSettings() { return m_ViewportDockSettings; }
	void setActive(bool enabled) { m_ViewportDockSettings.m_IsActive = enabled; }
};
