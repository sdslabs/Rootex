#pragma once

#include "common/common.h"
#include "core/resource_file.h"
#include "core/resource_data.h"

class ResourceManager
{
	static Vector<Ref<ResourceData>> s_ResourcesData;

	ResourceManager() = delete;
	~ResourceManager() = delete;

public:
	static void registerResourceData(Ref<ResourceData> resourceData);
	static ResourceData* getResourceData(unsigned int ID);
	static size_t getActiveCount();
	static void empty();	
};
