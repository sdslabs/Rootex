#pragma once

#include "system.h"

/// An interface for debug components.
class DebugSystem : public System
{
public:
	DebugSystem();

	void reportComponents();

#ifdef ROOTEX_EDITOR
	void draw() override;
#endif // ROOTEX_EDITOR
};
