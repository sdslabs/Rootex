#pragma once

#include "event_manager.h"

class InspectorDock
{
public:
	struct InspectorSettings
	{
		bool m_IsActive = true;
	};

private:
	InspectorSettings m_InspectorSettings;
	Entity* m_OpenedEntity;
	
	Variant openEntity(const Event* event);

public:
	InspectorDock();
	InspectorDock(InspectorDock&) = delete;
	~InspectorDock() = default;

	void draw();

	InspectorSettings& getSettings() { return m_InspectorSettings; }
	void setActive(bool enabled) { m_InspectorSettings.m_IsActive = enabled; }
};
