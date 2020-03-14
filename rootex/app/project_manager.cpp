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

void ProjectManager::openLevel(FilePath levelPath)
{
	PRINT("Loading level: " + levelPath.string());
	m_CurrentLevelName = levelPath.filename().string();

	EntityFactory::GetSingleton()->destroyEntities(true);
	Ref<RootHierarchyComponent> rootComponent = HierarchySystem::GetSingleton()->getRootHierarchyComponent();

	rootComponent = HierarchySystem::GetSingleton()->getRootHierarchyComponent();
	rootComponent->clear();

	for (auto&& entityFile : OS::GetFilesInDirectory((levelPath / "entities/").string()))
	{
		EntityFactory::GetSingleton()->createEntity(ResourceLoader::CreateTextResourceFile(entityFile.string()));
	}

	HierarchySystem::GetSingleton()->resetHierarchy();

	PRINT("Loaded level: " + levelPath.string());
}
