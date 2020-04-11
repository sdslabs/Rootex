#pragma once

#include "system.h"

/// Test for processing test components.
class TestSystem : public System
{
public:
	void update(float deltaMilliseconds);
};
