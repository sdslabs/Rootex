#pragma once

#include "common/common.h"
#include "resource_loader.h"

/// Helper for loading, saving and creating new projects.
class LevelManager
{
	LevelManager() = default;
	LevelManager(LevelManager&);
	~LevelManager() = default;
	String m_CurrentLevelName;
	TextResourceFile* m_CurrentLevelSettingsFile = nullptr;
	JSON::json m_CurrentLevelSettings;

public:
	static LevelManager* GetSingleton();
	void openLevel(const String& levelPath);
	void saveCurrentLevel();
	void createLevel(const String& newLevelName);

	/// Returns paths of all third-party libraries provided by rootex/vendor/.
	Vector<FilePath> getLibrariesPaths();
	String getCurrentLevelName() const { return m_CurrentLevelName; }
	JSON::json& getCurrentLevelSettings() { return m_CurrentLevelSettings; }
	TextResourceFile* getCurrentLevelSettingsFile() { return m_CurrentLevelSettingsFile; }
	bool isAnyLevelOpen() const { return m_CurrentLevelName != ""; }
};
