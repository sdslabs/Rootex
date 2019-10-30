#include "resource_manager.h"

Vector<Ptr<ResourceData>> ResourceManager::m_ResourcesData;

void ResourceManager::registerResourceDataResourceFile(Ptr<ResourceData> resourceData)
{
	auto it = std::find(m_ResourcesData.begin(), m_ResourcesData.end(), resourceData);
	if (it != m_ResourcesData.end())
	{
		return;
	}
	else
	{
		WARN("Cache miss while fetching resource data");
		m_ResourcesData.push_back(resourceData);
	}
}

ResourceData* ResourceManager::getResourceData(ResourceData* resource)
{
	auto it = std::find(m_ResourcesData.begin(), m_ResourcesData.end(), resource);
	if (it != m_ResourcesData.end())
	{
		return (*it).get();
	}

	m_ResourcesData.push_back(Ptr<ResourceData>(resource));

	return resource;
}

int ResourceManager::getActiveCount()
{
	return m_ResourcesData.size();
}

void ResourceManager::empty()
{
	for (auto& r : m_ResourcesData)
	{
		r = nullptr; // This also deletes the resource data handled by r
	}
	m_ResourcesData.clear();
}
