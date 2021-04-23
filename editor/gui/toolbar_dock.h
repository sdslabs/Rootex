#pragma once

#include "common/common.h"
#include "event_manager.h"

class ToolbarDock
{
	EventBinder<ToolbarDock> m_Binder;

	float m_LastUpdateTime = 0.0f;

public:
	struct ToolbarDockSettings
	{
		bool m_IsActive = true;
	};

private:
	ToolbarDockSettings m_ToolbarDockSettings;
	Vector<float> m_FPSRecords;
	unsigned int m_FPSRecordsPoolSize = 100;
	bool m_InEditorPlaying = false;
	String m_StartPlayingScene;

	Variant disablePlayInEditor(const Event* e);

public:
	ToolbarDock();
	ToolbarDock(ToolbarDock&) = delete;
	~ToolbarDock() = default;

	void draw(float deltaMilliseconds);

	ToolbarDockSettings& getSettings() { return m_ToolbarDockSettings; }
	void setActive(bool enabled) { m_ToolbarDockSettings.m_IsActive = enabled; }
};
