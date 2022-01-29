#pragma once

#include "framework/scene.h"
#include "framework/scene_loader.h"
#include "framework/system.h"

class PauseSystem : public System
{
	PauseSystem();
	PauseSystem(PauseSystem&) = delete;

private:
	bool m_IsPausingEnabled;

public:
	static PauseSystem* GetSingleton();

	bool& getIsPausingEnabled() { return m_IsPausingEnabled; }
	void setIsPausingEnabled(bool pausing) { m_IsPausingEnabled = pausing; }

	void update(float deltaMilliseconds) override;
};
