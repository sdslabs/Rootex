#pragma once

#include "common/common.h"
#include "core/resource_file.h"
#include "core/resource_data.h"

namespace ResourceDataReserve
{
extern Vector<Ref<ResourceData>> ResourcesData;

void registerResourceData(Ref<ResourceData> resourceData);
const ResourceData* getResourceData(unsigned int ID);
size_t getActiveCount();
void clear();	
};
