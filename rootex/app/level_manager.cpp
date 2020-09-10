#include "level_manager.h"

#include "core/input/input_manager.h"
#include "framework/entity_factory.h"
#include "framework/systems/hierarchy_system.h"
#include "framework/systems/render_system.h"
#include "systems/audio_system.h"
#include "systems/serialization_system.h"

LevelManager* LevelManager::GetSingleton()
{
	static LevelManager singleton;
	return &singleton;
}

void LevelManager::openLevel(const String& levelPath, bool openInEditor)
{
	if (isAnyLevelOpen())
	{
		endLevel();
	}

	m_CurrentLevelName = FilePath(levelPath).filename().string();
	m_CurrentLevelSettingsFile = ResourceLoader::CreateTextResourceFile(levelPath + "/" + m_CurrentLevelName + ".level.json");
	m_CurrentLevelSettings = JSON::json::parse(m_CurrentLevelSettingsFile->getString());

	if (!OS::IsExists(levelPath))
	{
		OS::CreateDirectoryName(levelPath);
	}
	if (!OS::IsExists(levelPath + "/entities/"))
	{
		OS::CreateDirectoryName(levelPath + "/entities/");
	}

	for (auto&& entityFile : OS::GetFilesInDirectory(levelPath + "/entities/"))
	{
		TextResourceFile* textResource = ResourceLoader::CreateTextResourceFile(entityFile.string());
		if (textResource->isDirty())
		{
			ResourceLoader::Reload(textResource);
		}

		EntityFactory::GetSingleton()->createEntity(textResource);
	}

	for (auto& [order, systems] : System::GetSystems())
	{
		for (auto& system : systems)
		{
			system->setConfig(m_CurrentLevelSettings, openInEditor);
		}
	}

	EntityFactory::GetSingleton()->setupLiveEntities();

	PRINT("Loaded level: " + levelPath);

	for (auto& [order, systems] : System::GetSystems())
	{
		for (auto& system : systems)
		{
			if (system->isActive())
			{
				system->begin();
			}
		}
	}
}

void LevelManager::saveCurrentLevel()
{
	SerializationSystem::GetSingleton()->saveAllEntities("game/assets/levels/" + getCurrentLevelName() + "/entities");
}

void LevelManager::saveCurrentLevelSettings()
{
	m_CurrentLevelSettingsFile->putString(m_CurrentLevelSettings.dump(1, '\t'));
}

void LevelManager::createLevel(const String& newLevelName)
{
	OS::CreateDirectoryName("game/assets/levels/" + newLevelName);
	OS::CreateDirectoryName("game/assets/levels/" + newLevelName + "/entities/");

	JSON::json newLevelJSON;
	newLevelJSON["camera"] = ROOT_ENTITY_ID;
	newLevelJSON["inputSchemes"] = JSON::json::array();
	newLevelJSON["startScheme"] = "";
	newLevelJSON["listener"] = ROOT_ENTITY_ID;
	OS::CreateFileName("game/assets/levels/" + newLevelName + "/" + newLevelName + ".level.json") << newLevelJSON.dump(1, '\t');

	PRINT("Created new level: " + "game/assets/levels/" + newLevelName);
}

void LevelManager::endLevel()
{
	if (isAnyLevelOpen())
	{
		for (auto& [order, systems] : System::GetSystems())
		{
			for (auto& system : systems)
			{
				system->end();
			}
		}

		EntityFactory::GetSingleton()->destroyEntities();
		HierarchySystem::GetSingleton()->getRootHierarchyComponent()->clear();

		PRINT("Ended level: " + m_CurrentLevelSettingsFile->getPath().generic_string());

		m_CurrentLevelName = "";
		m_CurrentLevelSettingsFile = nullptr;
		m_CurrentLevelSettings.clear();
	}
}

Vector<FilePath> LevelManager::getLibrariesPaths()
{
	return OS::GetDirectoriesInDirectory("rootex/vendor/");
}
