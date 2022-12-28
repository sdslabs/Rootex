#pragma once

#include "system.h"

/// Responsible for managing the notifications of trigger components.
class TriggerSystem : public System
{
	TriggerSystem();
	TriggerSystem(const TriggerSystem&) = delete;

public:
	static TriggerSystem* GetSingleton();

	void update(float deltaMilliseconds) override;
};
