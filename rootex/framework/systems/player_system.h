#pragma once

#include "common/common.h"
#include "framework/system.h"

class PlayerSystem : public System
{
	PlayerSystem();
	PlayerSystem(const PlayerSystem&) = delete;
	virtual ~PlayerSystem() = default;

public:
	static PlayerSystem* GetSingleton();

	bool initialize(const JSON::json& systemData) override;
	void setConfig(const SceneSettings& sceneSettings) override;

	void begin() override;
	void update(float deltaMilliseconds) override;
	void end() override;
};
