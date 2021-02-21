#pragma once

#include "common/common.h"
#include "framework/system.h"

#include "PostProcess.h"

class GameRenderSystem : public System
{
	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;

	GameRenderSystem();

public:
	static GameRenderSystem* GetSingleton();

	virtual bool initialize(const JSON::json& systemData) override;
	virtual void update(float deltaMilliseconds) override;
};
