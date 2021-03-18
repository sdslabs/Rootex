#pragma once

#include "system.h"

class TriggerSystem : public System
{
	TriggerSystem();
	TriggerSystem(const TriggerSystem&) = delete;
	~TriggerSystem() = default;

public:
	static TriggerSystem* GetSingleton();

	void update(float deltaMilliseconds) override;
};
