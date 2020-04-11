#pragma once

#include "system.h"

/// An interface for debug components.
class DebugSystem : public System
{
public:
	void update(float deltaMilliseconds);
};
