#include "serialization_system.h"

#include "common/common.h"
#include "entity.h"
#include "component.h"
#include "entity_factory.h"

SerializationSystem* SerializationSystem::GetSingleton()
{
	static SerializationSystem singleton;
	return &singleton;
}

void SerializationSystem::saveAllEntities(const String& dirPath)
{
	if (!OS::IsExists(dirPath))
	{
		OS::CreateDirectoryName(dirPath);
	}

	for (auto&& entity : EntityFactory::GetSingleton()->getEntities())
	{
		if (entity.second->getID() != ROOT_ENTITY_ID)
		{
			InputOutputFileStream file = OS::CreateFileName(dirPath + "/" + entity.second->getName() + "_" + std::to_string(entity.second->getID()) + ".entity.json");
			file << std::setw(4) << entity.second->getJSON() << std::endl;
		}
	}
}
