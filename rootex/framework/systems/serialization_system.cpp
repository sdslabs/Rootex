#include "serialization_system.h"

#include "common/common.h"
#include "entity.h"
#include "component.h"
#include "entity_factory.h"

void SerializationSystem::saveAllEntities(const String& dirPath, const String& projectName)
{
	if (!OS::IsExists(dirPath))
	{
		OS::CreateDirectoryName(dirPath);
	}

	for (auto&& entity : EntityFactory::GetSingleton()->getEntities())
	{
		InputOutputFileStream file = OS::CreateFileName(dirPath + "/" + entity->getName() + ".entity.json");
		file << std::setw(4) << entity->getJSON() << std::endl;
	}
}
