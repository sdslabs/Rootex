#pragma once

#include <vector>
#include "resource.h"

class ResourceManager 
{
	static std::vector<Resource*> m_Resources;

	friend ResourceLoader;

public:	
	static void registerResource(Resource* resource);
	static int getActiveCount();
	static void emptyAll();
};
