#pragma once

#include "common/common.h"
#include "core/resource_loader.h"
#include "core/resource_files/text_resource_file.h"

class LevelDescription
{
	String m_LevelName;
	TextResourceFile* m_LevelSettingsFile = nullptr;
	JSON::json m_LevelSettings;
	Vector<String> m_Preloads;
	Vector<String> m_Arguments;

public:
	LevelDescription();
	LevelDescription(const String& levelPath, const Vector<String>& arguments);

	String getLevelName() const { return m_LevelName; }
	JSON::json& getLevelSettings() { return m_LevelSettings; }
	TextResourceFile* getLevelSettingsFile() { return m_LevelSettingsFile; }
	Vector<String>& getPreloads() { return m_Preloads; }
	const Vector<String>& getArguments() const { return m_Arguments; }
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
	static void RegisterAPI(sol::table& rootex);
	static LevelManager* GetSingleton();
	/// Load resources marked as to be preloaded in the level. Use as a loading screen helper.
	/// Returns the number of resources loaded
	int preloadLevel(const String& levelPath, Atomic<int>& progress, bool openInEditor = false);
	void openPreloadedLevel(const String& levelPath, const Vector<String>& arguments, bool openInEditor);
	
	/// Open an entire level in one go. This performs preloading on its own.
	void openLevel(const String& levelPath, const Vector<String>& arguments, bool openInEditor = false);
	
	void saveCurrentLevel();
	void saveCurrentLevelSettings();
	
	void createLevel(const String& newLevelName);

	bool isAnyLevelOpen() const { return m_CurrentLevel.getLevelName() != ""; }

	LevelDescription& getCurrentLevel() { return m_CurrentLevel; }
};
