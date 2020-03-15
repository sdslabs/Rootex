#include "project_manager.h"

#include "framework/entity_factory.h"
#include "framework/components/root_hierarchy_component.h"
#include "framework/systems/hierarchy_system.h"
#include "resource_loader.h"

ProjectManager* ProjectManager::GetSingleton()
{
	static ProjectManager singleton;
	return &singleton;
}

void ProjectManager::openLevel(String levelPath)
{
	PRINT("Loading level: " + levelPath);
	m_CurrentLevelName = FilePath(levelPath).filename().string();

	EntityFactory::GetSingleton()->destroyEntities(true);
	Ref<RootHierarchyComponent> rootComponent = HierarchySystem::GetSingleton()->getRootHierarchyComponent();

	rootComponent = HierarchySystem::GetSingleton()->getRootHierarchyComponent();
	rootComponent->clear();

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
