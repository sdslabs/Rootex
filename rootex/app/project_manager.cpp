#include "project_manager.h"

#include "framework/entity_factory.h"
#include "framework/systems/hierarchy_system.h"
#include "resource_loader.h"

#include "systems/serialization_system.h"

ProjectManager* ProjectManager::GetSingleton()
{
	static ProjectManager singleton;
	return &singleton;
}

void ProjectManager::openLevel(const String& levelPath)
{
	m_CurrentLevelName = FilePath(levelPath).filename().string();

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

	PRINT("Loaded level: " + levelPath);
}

void ProjectManager::saveCurrentLevel()
{
	SerializationSystem::GetSingleton()->saveAllEntities("game/assets/levels/" + getCurrentLevelName() + "/entities");
}

void ProjectManager::createLevel(const String& newLevelName)
{
	OS::CreateDirectoryName("game/assets/levels/" + newLevelName);
	OS::CreateDirectoryName("game/assets/levels/" + newLevelName + "/entities/");
	PRINT("Created new level: " + "game/assets/levels/" + newLevelName);
}

Vector<FilePath> ProjectManager::getLibrariesPaths()
{
	return OS::GetDirectoriesInDirectory("rootex/vendor/");
}
