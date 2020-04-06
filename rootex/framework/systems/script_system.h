#pragma once

#include "system.h"

class ScriptSystem : public System
{
	ScriptSystem() = default;
	ScriptSystem(ScriptSystem&) = delete;
	~ScriptSystem() = default;

public:
	static ScriptSystem* GetSingleton();

	void begin();
	void update(float deltaMilliseconds);
	void end();
};
