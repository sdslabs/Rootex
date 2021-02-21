#pragma once

#include "event_manager.h"
#include "framework/ecs_factory.h"
#include "framework/component.h"

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
	Scene* m_OpenedScene = nullptr;
	String m_OpenedSceneName;
	bool m_IsNameBeingEdited;
	Scene* m_ActionScene = nullptr;

	Variant openScene(const Event* event);
	Variant closeScene(const Event* event);

public:
	static InspectorDock* GetSingleton() { return s_Singleton; };

	InspectorDock();
	InspectorDock(InspectorDock&) = delete;
	~InspectorDock() = default;

	void draw(float deltaMilliseconds);

	void drawSceneActions(Scene* scene);
	Scene* getOpenedScene() { return m_OpenedScene; }
	InspectorSettings& getSettings() { return m_InspectorSettings; }
	void setActive(bool enabled) { m_InspectorSettings.m_IsActive = enabled; }
};
