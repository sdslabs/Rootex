#include "resource_manager.h"

std::vector<IResource*> ResourceManager::m_Resources;

void ResourceManager::registerResource(IResource* resource)
{
	m_Resources.push_back(resource);
}

int ResourceManager::getActiveCount()
{
	return m_Resources.size();
}

void ResourceManager::emptyAll()
{
	for (auto r : m_Resources)
	{
		delete r;
	}
	m_Resources.clear();
}
