#include "resource_manager.h"

Vector<Ref<ResourceData>> ResourceManager::s_ResourcesData;

void ResourceManager::registerResourceData(Ref<ResourceData> resourceData)
{
	auto it = std::find(s_ResourcesData.begin(), s_ResourcesData.end(), resourceData);
	if (it != s_ResourcesData.end())
	{
		return;
	}
	else
	{
		WARN("ResourceManager: Cache miss while fetching resource data");
		s_ResourcesData.push_back(resourceData);
	}
}

ResourceData* ResourceManager::getResourceData(unsigned int ID)
{
	if (ID < s_ResourcesData.size())
	{
		return s_ResourcesData[ID].get();
	}
	return nullptr;
}

size_t ResourceManager::getActiveCount()
{
	return s_ResourcesData.size();
}

void ResourceManager::empty()
{
	for (auto& r : s_ResourcesData)
	{
		r.reset(); // This also deletes the resource data handled by r
	}
	s_ResourcesData.clear();
}
