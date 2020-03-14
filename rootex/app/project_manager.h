#pragma once

#include "common/common.h"

class ProjectManager
{
	ProjectManager() = default;
	ProjectManager(ProjectManager&);
	~ProjectManager() = default;

	String m_CurrentLevelName;

public:
	static ProjectManager* GetSingleton();

	void openLevel(String levelPath);
	String getCurrentLevelName() const { return m_CurrentLevelName; }
	bool isAnyLevelOpen() const { return m_CurrentLevelName != ""; }
};
