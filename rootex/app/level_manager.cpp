#include "level_manager.h"

#include "framework/entity_factory.h"
#include "framework/systems/hierarchy_system.h"
#include "framework/systems/render_system.h"
#include "systems/serialization_system.h"
#include "core/input/input_manager.h"

LevelManager* LevelManager::GetSingleton()
{
	static LevelManager singleton;
	return &singleton;
}

void LevelManager::openLevel(const String& levelPath, bool openInEditor)
{
	m_CurrentLevelName = FilePath(levelPath).filename().string();
	m_CurrentLevelSettingsFile = ResourceLoader::CreateTextResourceFile(levelPath + "/" + m_CurrentLevelName + ".level.json");
	m_CurrentLevelSettings = JSON::json::parse(m_CurrentLevelSettingsFile->getString());

	EntityFactory::GetSingleton()->destroyEntities(true);
	HierarchySystem::GetSingleton()->getRootHierarchyComponent()->clear();

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
			textResource->reload();
		}

		EntityFactory::GetSingleton()->createEntity(textResource);
	}	

	HierarchySystem::GetSingleton()->resetHierarchy();

	if (m_CurrentLevelSettings.find("camera") != m_CurrentLevelSettings.end())
	{
		Ref<Entity> cameraEntity = EntityFactory::GetSingleton()->findEntity(m_CurrentLevelSettings["camera"]);
		RenderSystem::GetSingleton()->setCamera(cameraEntity->getComponent<CameraComponent>().get());
	}

	if (!openInEditor)
	{
		JSON::json& levelJSON = getCurrentLevelSettings();
		InputManager::GetSingleton()->loadSchemes(levelJSON["inputSchemes"]);

		if (levelJSON.find("startScheme") != levelJSON.end())
		{
			InputManager::GetSingleton()->setScheme(levelJSON["startScheme"]);
		}
	}

	EntityFactory::GetSingleton()->setupLiveEntities();

	PRINT("Loaded level: " + levelPath);
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
	OS::CreateFileName("game/assets/levels/" + newLevelName + "/" + newLevelName + ".level.json") << newLevelJSON.dump(1, '\t');
	
	PRINT("Created new level: " + "game/assets/levels/" + newLevelName);
}

Vector<FilePath> LevelManager::getLibrariesPaths()
{
	return OS::GetDirectoriesInDirectory("rootex/vendor/");
}
