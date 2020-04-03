#pragma once

#include "event_manager.h"
#include "rootex/framework/component.h"

class InspectorDock
{
public:
	struct InspectorSettings
	{
		bool m_IsActive = true;
	};

private:
	constexpr static unsigned int s_InputTextBufferSize = 256;

	InspectorSettings m_InspectorSettings;
	Ref<Entity> m_OpenedEntity;
	String m_OpenedEntityName;
	bool m_IsNameBeingEdited;
	Vector<Tuple<ComponentID, String, bool>> m_AddNewComponentSelectionCache;
	
	Variant openEntity(const Event* event);

	void drawAddComponentWindow();
	void drawRemoveComponentWindow();
	void refreshAddNewComponentSelectionCache();

public:
	InspectorDock();
	InspectorDock(InspectorDock&) = delete;
	~InspectorDock() = default;

	void draw();

	InspectorSettings& getSettings() { return m_InspectorSettings; }
	void setActive(bool enabled) { m_InspectorSettings.m_IsActive = enabled; }
};
