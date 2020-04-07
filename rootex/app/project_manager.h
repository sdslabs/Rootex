#pragma once

#include "common/common.h"

/// Helper for loading, saving and creating new projects.
class ProjectManager
{
	ProjectManager() = default;
	ProjectManager(ProjectManager&);
	~ProjectManager() = default;


public:
	String m_CurrentLevelName;
	static ProjectManager* GetSingleton();

	void openLevel(const String& levelPath);
	void saveCurrentLevel();
	void createLevel(const String& newLevelName);

	/// Returns paths of all third-party libraries provided by rootex/vendor/.
	Vector<FilePath> getLibrariesPaths();
	String getCurrentLevelName() const { return m_CurrentLevelName; }
	bool isAnyLevelOpen() const { return m_CurrentLevelName != ""; }
};
