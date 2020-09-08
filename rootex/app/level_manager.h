#pragma once

#include "common/common.h"
#include "resource_loader.h"

class LevelDescription
{
	String m_LevelName;
	TextResourceFile* m_LevelSettingsFile = nullptr;
	JSON::json m_LevelSettings;
	Vector<String> m_Preloads;

public:
	LevelDescription();
	LevelDescription(const String& levelPath);

	String getLevelName() const { return m_LevelName; }
	JSON::json& getLevelSettings() { return m_LevelSettings; }
	TextResourceFile* getLevelSettingsFile() { return m_LevelSettingsFile; }
	Vector<String>& getPreloads() { return m_Preloads; }
};

/// Helper for loading, saving and creating new projects.
class LevelManager
{
	LevelManager() = default;
	LevelManager(LevelManager&);
	~LevelManager() = default;

	LevelDescription m_CurrentLevel;
	Vector<String> m_ToUnload;

	void endLevel();

public:
	static LevelManager* GetSingleton();
	/// Load resources marked as to be preloaded in the level. Use as a loading screen helper.
	/// Returns the number of resources loaded
	int preloadLevel(const String& levelPath, Atomic<int>& progress, bool openInEditor = false);
	void openPreloadedLevel(const String& levelPath, bool openInEditor);
	
	/// Open an entire level in one go. This performs preloading on its own.
	void openLevel(const String& levelPath, bool openInEditor = false);
	
	void saveCurrentLevel();
	void saveCurrentLevelSettings();
	
	void createLevel(const String& newLevelName);

	bool isAnyLevelOpen() const { return m_CurrentLevel.getLevelName() != ""; }

	LevelDescription& getCurrentLevel() { return m_CurrentLevel; }
};
