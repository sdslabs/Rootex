#pragma once

#include "common/common.h"
#include "core/resource.h"
#include "core/resource_data.h"

class ResourceManager
{
	static HashMap<Ptr<IResourceData>, Vector<IResourceFile>> m_ResourceMap;

public:
	static void registerResourceDataResourceFile(Ptr<IResourceData> resourceData, Ptr<IResourceFile> resourceFile);
	static int getActiveCount();
	static void empty();
};
