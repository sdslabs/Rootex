#include "serialization_system.h"

#include "common/common.h"
#include "entity.h"
#include "component.h"
#include "entity_factory.h"

SerializationSystem::SerializationSystem()
    : System("SerializationSystem", UpdateOrder::Async, false)
{
}

SerializationSystem* SerializationSystem::GetSingleton()
{
	static SerializationSystem singleton;
	return &singleton;
}

void SerializationSystem::saveAllEntities(const String& dirPath)
{
	String cachePath = dirPath + ".cache";

	bool retry = false;
	if (OS::IsExists(cachePath))
	{
		WARN("Last save was not completed. Retrying with a fresh save cache");
		
		if (!OS::DeleteDirectory(cachePath)) 
		{
			retry = true;
		}
	}

	OS::CreateDirectoryName(cachePath);

	for (auto&& entity : EntityFactory::GetSingleton()->getEntities())
	{
		if (entity.second->getID() != ROOT_ENTITY_ID && !entity.second->isEditorOnly())
		{
			InputOutputFileStream file = OS::CreateFileName(cachePath + "/" + entity.second->getFullName() + ".entity.json");
			file << std::setw(4) << entity.second->getJSON() << std::endl;
		}
	}

	if (!OS::DeleteDirectory(dirPath)) 
	{
		retry = true;
	}
	if (!OS::Rename(cachePath, dirPath)) 
	{
		retry = true;
	}

	if (retry)
	{
		WARN("Retrying save level: " + dirPath);
		saveAllEntities(dirPath);
	}
}
