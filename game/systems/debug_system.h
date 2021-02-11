#pragma once

#include "system.h"

class Scene;

class DebugSystem : public System
{
	bool m_IsEditing = false;

	DebugSystem();
	DebugSystem(DebugSystem&) = delete;
	~DebugSystem() = default;

	void drawSceneTree(Scene* scene);

public:
	static DebugSystem* GetSingleton();

	bool initialize(const JSON::json& systemData) override;
	void update(float deltaMilliseconds) override;
};
