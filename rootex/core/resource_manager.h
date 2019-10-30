#pragma once

#include "common/common.h"
#include "core/resource_file.h"
#include "core/resource_data.h"

class ResourceManager
{
	static Vector<Ptr<ResourceData>> m_ResourcesData;

	ResourceManager() = delete;

public:
	static void registerResourceDataResourceFile(Ptr<ResourceData> resourceData);
	static ResourceData* getResourceData(ResourceData* resource);
	static int getActiveCount();
	static void empty();	
};
