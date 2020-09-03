#pragma once

#include "system.h"

/// An interface for debug components.
class DebugSystem : public System
{
public:
	DebugSystem();

	void update(float deltaMilliseconds) override;
};
