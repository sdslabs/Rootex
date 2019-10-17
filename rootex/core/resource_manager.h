#pragma once

#include <vector>
#include "resource.h"

class ResourceManager 
{
	static std::vector<IResource*> m_Resources;

	friend ResourceLoader;

public:	
	static void registerResource(IResource* resource);
	static int getActiveCount();
	static void emptyAll();
};
