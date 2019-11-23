#include "resource_data_reserve.h"

namespace ResourceDataReserve
{
Vector<Ref<ResourceData>> ResourcesData;

void registerResourceData(Ref<ResourceData> resourceData)
{
	auto it = std::find(ResourcesData.begin(), ResourcesData.end(), resourceData);
	if (it != ResourcesData.end())
	{
		return;
	}
	else
	{
		WARN("ResourceManager: Cache miss while fetching resource data");
		ResourcesData.push_back(resourceData);
	}
}

const ResourceData* getResourceData(unsigned int ID)
{
	if (ID < ResourcesData.size())
	{
		return ResourcesData[ID].get();
	}
	return nullptr;
}

size_t getActiveCount()
{
	return ResourcesData.size();
}

void clear()
{
	for (auto& r : ResourcesData)
	{
		r.reset(); // This also deletes the resource data handled by r
	}
	ResourcesData.clear();
}
}
