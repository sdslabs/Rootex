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
	static InspectorDock* s_Singleton;
	constexpr static unsigned int s_InputTextBufferSize = 256;

	InspectorSettings m_InspectorSettings;
	Ref<Entity> m_OpenedEntity;
	String m_OpenedEntityName;
	bool m_IsNameBeingEdited;
	String m_MenuAction;
	Ref<Entity> m_ActionEntity;
	Vector<Tuple<ComponentID, String, bool>> m_AddNewComponentSelectionCache;
	
	Variant openEntity(const Event* event);
	Variant closeEntity(const Event* event);

	void drawAddComponentWindow();
	void drawRemoveComponentWindow();
	void refreshAddNewComponentSelectionCache();

public:
	static InspectorDock* GetSingleton() { return s_Singleton; };

	InspectorDock();
	InspectorDock(InspectorDock&) = delete;
	~InspectorDock() = default;

	void draw(float deltaMilliseconds);

	void drawEntityActions(Ref<Entity> actionEntity);
	Ref<Entity> getOpenedEntity() { return m_OpenedEntity; }
	InspectorSettings& getSettings() { return m_InspectorSettings; }
	void setActive(bool enabled) { m_InspectorSettings.m_IsActive = enabled; }
};
